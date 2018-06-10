#ifndef MEMORY_FIXED_BLOCK_ARRAY
#define MEMORY_FIXED_BLOCK_ARRAY

#include <cassert>
#include <stdint.h>

namespace memory
{
    template <typename T, uint32_t size>
    class FixedBlockArray
    {
    private:
        static const uint32_t INDEX_NONE = UINT32_MAX;

        union Element
        {
            T value;
            uint32_t next;

            //Glorious hack coming through. This badboy trivial constructor
            //exists so that we can use glorious unions for type T.
            Element()
            {
                memset(this, 0, sizeof(*this));
            }
        };

        Element _array[size];
        uint32_t _freeList;

    public:
        FixedBlockArray()
        {
            _freeList = 0;

            //Initialize indexed linked list
            for (uint32_t i = 0; i < size - 1; i++)
            {
                _array[i].next = i + 1;   //Point to the next block
            }

            //Make last one null
            _array[size - 1].next = INDEX_NONE;
        }

        ~FixedBlockArray()
        {
        }

        uint32_t allocIndex()
        {
            if (_freeList == INDEX_NONE)
            {
                return INDEX_NONE;
            }
            else
            {
                uint32_t retIdx = _freeList;

                //Move free list to next index
                _freeList = _array[retIdx].next;

                return retIdx;
            }
        }

        void freeIndex(uint32_t index)
        {
            assert(index >= 0);
            assert(index < size);

            if (_freeList == INDEX_NONE)
            {
                _freeList = index;

                //Make sure we're not accidentally pointing to garbage!
                _array[index].next = INDEX_NONE;
            }
            else
            {
                uint32_t oldIndex = _freeList;
                _freeList = index;

                //Point to old head node
                _array[index].next = oldIndex;
            }
        }

        T& operator[](uint32_t index)
        {
            assert(index >= 0);
            assert(index < size);
            return _array[index].value;
        }

        const T& operator[](uint32_t index) const
        {
            assert(index >= 0);
            assert(index < size);
            return _array[index].value;
        }
    };
}

#endif
