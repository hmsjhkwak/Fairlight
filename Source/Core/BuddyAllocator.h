#ifndef MEMORY_BUDDY_ALLOCATOR_H
#define MEMORY_BUDDY_ALLOCATOR_H

#include <stdint.h>

//#define VERIFY_BUDDY_ALLOCATOR

namespace memory
{
    struct BuddyNode;
    class BuddyAllocator
    {
    private:
        static const int MAX_LEVELS = 32;

        void* _memory;
        BuddyNode* _freeLists[MAX_LEVELS];
        size_t _leafSize;
        size_t _allocatedSize;
        uint32_t _levels;

    public:
        BuddyAllocator();
        ~BuddyAllocator();

        void init(size_t leafSize, uint32_t levels);

        void* alloc(size_t size);
        void free(void* ptr, size_t size);

        void* getBaseAddress();
        size_t getActualAllocSize(size_t size);

    private:
        void* allocBlock(uint32_t level);
        void freeBlock(uint32_t level, uint32_t levelIndex);

        bool isBlockAllocated(uint32_t blockIndex);
        void markBlockAsAllocated(uint32_t blockIndex);
        void markBlockAsUnallocated(uint32_t blockIndex);

        size_t getBlockSize(uint32_t level);
        uint32_t getLevel(size_t size);
        uint32_t getLevelIndex(void* ptr, uint32_t level);
        uint32_t getBlockIndex(uint32_t levelIndex, uint32_t level);
        uint32_t getBlockIndex(void* ptr, uint32_t level);

#ifdef VERIFY_BUDDY_ALLOCATOR
        void verify();
#endif
    };
}

#endif
