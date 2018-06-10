#include "Core/Core.h"
#include "BuddyAllocator.h"
#include <cassert>
#include <cstdlib>
#include <cmath>
#include <cstring>

namespace memory
{
    inline bool isPowerOfTwo(uint32_t x)
    {
        return !(x & (x - 1));
    }

    uint32_t nextPowerOfTwo(uint32_t x)
    {
        if (isPowerOfTwo(x)) { return x; }

        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;

        return x + 1;
    }



    struct BuddyNode
    {
        BuddyNode* next;
        BuddyNode* prev;
    };

    BuddyAllocator::BuddyAllocator() :
        _memory(nullptr),
        _leafSize(0),
        _allocatedSize(0),
        _levels(0)
    {
        for (int i = 0; i < MAX_LEVELS; i++)
        {
            _freeLists[i] = nullptr;
        }
    }

    BuddyAllocator::~BuddyAllocator()
    {
        std::free(_memory);
        _memory = nullptr;
    }

    void BuddyAllocator::init(size_t leafSize, uint32_t levels)
    {
        assert(_memory == nullptr);     //Make sure this isn't initialized twice
        assert(levels > 1);
        assert(levels < MAX_LEVELS);

        //Needs to be large enough to store two pointers
        _leafSize = leafSize;
        if (_leafSize < 16) { _leafSize = 16; }

        _levels = levels;
        const uint32_t totalLeafNodes = 1 << (_levels - 1);
        _allocatedSize = _leafSize * totalLeafNodes;
        _memory = malloc(_allocatedSize);

        //Initialize free lists. This is kind of tricky because we are storing
        //the metadata bits that mark blocks as free/full at the beginning of
        //the buffer. First things first, let's calculate the number of leaf
        //nodes that this metadata will occupy.
        uint32_t leafNodes = (uint32_t)ceil((float)totalLeafNodes / 4 / (float)_leafSize);
        void* leafNodePtr = _memory;    //This is also used below for setting
                                        //up the free blocks

        //Zero out the leaf nodes to mark all as free
        memset(_memory, 0, leafNodes * _leafSize);

        //Now mark all of those leaf nodes as occupied
        for (uint32_t i = 0; i < leafNodes; i++)
        {
            //Loop to mark "parent" blocks as allocated
            uint32_t blockIndex = getBlockIndex(leafNodePtr, _levels - 1);
            while (blockIndex > 0)
            {
                markBlockAsAllocated(blockIndex);
                blockIndex >>= 1;
            }

            leafNodePtr = (void*)((uint8_t*)leafNodePtr + _leafSize);
        }

        //Now we need to build the list of free nodes. First, we calculate how
        //many free nodes are left over. We'll use that number's binary
        //representation to quickly determine the first free nodes.
        uint32_t leafNodesLeft = totalLeafNodes - leafNodes;

        //Now we work our way from the ones place up and mark certain blocks as
        //free if the bit happens to be set.
        uint32_t curLevel = _levels - 1;
        while (leafNodesLeft > 0)
        {
            if ((leafNodesLeft & 1) == 1)
            {
                //Add block to free list
                BuddyNode* node = (BuddyNode*)leafNodePtr;
                node->next = nullptr;
                node->prev = nullptr;
                _freeLists[curLevel] = node;
                leafNodePtr = (void*)((uint8_t*)leafNodePtr + getBlockSize(curLevel));
            }

            leafNodesLeft >>= 1;
            curLevel--;
        }
    }

    void* BuddyAllocator::alloc(size_t size)
    {
#ifdef VERIFY_BUDDY_ALLOCATOR
        verify();
#endif

        //Make sure we have enough space
        if (size > _allocatedSize) { return nullptr; }

        int level = getLevel(size);
        void* ret = allocBlock(level);

#ifdef VERIFY_BUDDY_ALLOCATOR
        verify();
#endif
        return ret;
    }

    void* BuddyAllocator::allocBlock(uint32_t level)
    {
        //If we don't have a free block, start splitting
        if (_freeLists[level] == nullptr)
        {
            if (level < 1)
            {
                //Unable to meet request
                return nullptr;
            }
            else
            {
                //Get a block twice the size that we need
                void* bigBlock = allocBlock(level - 1);
                if (bigBlock == nullptr) { return nullptr; }

                size_t blockSize = getBlockSize(level);
                void* smallBlock1 = bigBlock;
                BuddyNode* smallBlock2 = (BuddyNode*)((uint8_t*)bigBlock + blockSize);

                //Add the second block to the free list
                _freeLists[level] = smallBlock2;
                smallBlock2->next = nullptr;
                smallBlock2->prev = nullptr;

                markBlockAsAllocated(getBlockIndex(smallBlock1, level));

                return (void*)smallBlock1;
            }
        }
        else
        {
            BuddyNode* node = _freeLists[level];

            //Update free list
            BuddyNode* next = node->next;
            _freeLists[level] = next;
            if (next != nullptr)
            {
                next->prev = nullptr;
            }

            markBlockAsAllocated(getBlockIndex(node, level));

            return node;
        }
    }

    void BuddyAllocator::free(void* ptr, size_t size)
    {
#ifdef VERIFY_BUDDY_ALLOCATOR
        verify();
#endif

        uint32_t level = getLevel(size);
        uint32_t levelIndex = getLevelIndex(ptr, level);

        freeBlock(level, levelIndex);

#ifdef VERIFY_BUDDY_ALLOCATOR
        verify();
#endif
    }

    void BuddyAllocator::freeBlock(uint32_t level, uint32_t levelIndex)
    {
        uint32_t buddyLevelIndex = levelIndex ^ 1;
        uint32_t buddyBlockIndex = getBlockIndex(buddyLevelIndex, level);

        markBlockAsUnallocated(getBlockIndex(levelIndex, level));

        if (isBlockAllocated(buddyBlockIndex))
        {
            //Cannot merge the buddies; add to free list
            void* ptr = (char*)_memory + levelIndex * getBlockSize(level);

            BuddyNode* node = (BuddyNode*)ptr;
            node->next = _freeLists[level];
            node->prev = nullptr;

            if (node->next != nullptr)
            {
                node->next->prev = node;
            }

            _freeLists[level] = node;
        }
        else
        {
            //Remove the buddy from the free list. We're going to try to add
            //the parent to the free list (and maybe it's parent, and so on...)
            BuddyNode* currentNode = _freeLists[level];
            while (getLevelIndex(currentNode, level) != buddyLevelIndex)
            {
                currentNode = currentNode->next;
            }

            //Remove from list
            if (currentNode->prev == nullptr)
            {
                _freeLists[level] = currentNode->next;
                if (currentNode->next != nullptr)
                {
                    currentNode->next->prev = nullptr;
                }
            }
            else
            {
                currentNode->prev->next = currentNode->next;
                if (currentNode->next != nullptr)
                {
                    currentNode->next->prev = currentNode->prev;
                }
            }

            //Also mark buddy as free
            markBlockAsUnallocated(buddyBlockIndex);

            //Recurse
            freeBlock(level - 1, levelIndex >> 1);
        }
    }

    void* BuddyAllocator::getBaseAddress()
    {
        return _memory;
    }

    size_t BuddyAllocator::getActualAllocSize(size_t size)
    {
        int npot = nextPowerOfTwo((uint32_t)ceilf((float)size / (float)_leafSize));
        return npot * _leafSize;
    }



    bool BuddyAllocator::isBlockAllocated(uint32_t blockIndex)
    {
        uint32_t byte = blockIndex / 8;
        uint32_t bit = blockIndex % 8;
        return (((uint8_t*)_memory)[byte] >> bit) & 1;
    }

    void BuddyAllocator::markBlockAsAllocated(uint32_t blockIndex)
    {
        uint32_t byte = blockIndex / 8;
        uint32_t bit = blockIndex % 8;
        ((uint8_t*)_memory)[byte] |= (1 << bit);
    }

    void BuddyAllocator::markBlockAsUnallocated(uint32_t blockIndex)
    {
        uint32_t byte = blockIndex / 8;
        uint32_t bit = blockIndex % 8;
        ((uint8_t*)_memory)[byte] &= ~(1 << bit);
    }



    size_t BuddyAllocator::getBlockSize(uint32_t level)
    {
        return _allocatedSize / ((size_t)1 << level);
    }

    uint32_t BuddyAllocator::getLevel(size_t size)
    {
        int npot = nextPowerOfTwo((uint32_t)ceilf((float)size / (float)_leafSize));
        return _levels - 1 - (uint32_t)(log(npot) / log(2));
    }

    inline uint32_t BuddyAllocator::getLevelIndex(void* ptr, uint32_t level)
    {
        return (uint32_t)(((char*)ptr - (char*)_memory) / getBlockSize(level));
    }

    inline uint32_t BuddyAllocator::getBlockIndex(uint32_t levelIndex, uint32_t level)
    {
        return levelIndex + (1 << level);
    }

    inline uint32_t BuddyAllocator::getBlockIndex(void* ptr, uint32_t level)
    {
        return getBlockIndex(getLevelIndex(ptr, level), level);
    }

#ifdef VERIFY_BUDDY_ALLOCATOR
    void BuddyAllocator::verify()
    {
        //Iterate through each bit of metadata
        //Verify that the free lists are correct
        uint32_t maxBlockIndex = (1 << _levels) - 1;
        for (uint32_t blockIndex = 1; blockIndex < maxBlockIndex; blockIndex++)
        {
            uint32_t level = (uint32_t)(log(blockIndex) / log(2));
            if (isBlockAllocated(blockIndex))
            {
                //Allocated: Make sure it's not in the free list
                BuddyNode* current = _freeLists[level];
                while (current != nullptr)
                {
                    if (getBlockIndex(current, level) == blockIndex)
                    {
                        //Shouldn't be in the list
                        assert(false);
                    }
                    current = current->next;
                }
            }
            else
            {
                //Unallocated: Make sure it (or its parent) is in the free list
                bool inFreeList = false;
                uint32_t bIdx = blockIndex;
                while (level > 0)
                {
                    BuddyNode* current = _freeLists[level];
                    while (current != nullptr)
                    {
                        if (getBlockIndex(current, level) == bIdx)
                        {
                            inFreeList = true;
                            break;
                        }
                        current = current->next;
                    }

                    if (inFreeList) { break; }  //Break early
                    if (isBlockAllocated(bIdx))
                    {
                        //Totally inaccurate, but eh...
                        //Occurs when a large block is allocated
                        //and the smaller block flags are not set
                        inFreeList = true;
                        break;
                    }

                    level--;
                    bIdx >>= 1;
                }

                if (!inFreeList)
                {
                    //It should be there...
                    assert(false);
                }
            }
        }

        //Walk free lists to make sure that the pointers are valid
        //If they're not, we'll crash...
        for (int i = 0; i < _levels; i++)
        {
            BuddyNode* current = _freeLists[i];

            //Walk forward
            while (current != nullptr && current->next != nullptr)
            {
                current = current->next;
            }

            //Walk backward
            while (current != nullptr)
            {
                current = current->prev;
            }
        }
    }
#endif
}
