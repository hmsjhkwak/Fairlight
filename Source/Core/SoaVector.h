#ifndef MEMORY_SOA_VECTOR_H
#define MEMORY_SOA_VECTOR_H

#define CLASS_SOA_VECTOR2(className, type1, name1, type2, name2) \
    class className \
    { \
    private: \
        static_assert(alignof(type1) >= alignof(type2), \
            "Types must be listed in descending alignment."); \
        void* _memory; \
        uint32_t _size; \
        uint32_t _capacity; \
        void internalResize(uint32_t newCapacity) \
        { \
            const uint32_t TOTAL_ELEMENT_SIZE = sizeof(type1) + sizeof(type2); \
            const uint32_t ALIGNMENT = alignof(type1); \
            void* newMemory = nullptr; \
            type1* newName1 = nullptr; \
            type2* newName2 = nullptr; \
            if (newCapacity > 0) \
            { \
                newMemory = _aligned_malloc(TOTAL_ELEMENT_SIZE * newCapacity, ALIGNMENT); \
                newName1 = (type1*)newMemory; \
                newName2 = (type2*)(newName1 + newCapacity); \
            } \
            if (newCapacity < _size) { _size = newCapacity; } \
            if (_memory != nullptr && newMemory != nullptr) \
            { \
                memcpy(newName1, name1, sizeof(type1) * _size); \
                memcpy(newName2, name2, sizeof(type2) * _size); \
            } \
            _aligned_free(_memory); \
            _memory = newMemory; \
            name1 = newName1; \
            name2 = newName2; \
            _capacity = newCapacity; \
        } \
    public: \
        type1* name1; \
        type2* name2; \
        className() : _memory(nullptr), _size(0), _capacity(0) { } \
        className(uint32_t capacity) : _memory(nullptr), _size(0), _capacity(0) \
        { \
            internalResize(capacity); \
        } \
        ~className() \
        { \
            _aligned_free(_memory); \
            _size = 0; \
            _capacity = 0; \
        } \
        void push(type1 n1, type2 n2) \
        { \
            if (_size == _capacity) \
            { \
                if (_capacity == 0) { internalResize(32); } \
                else { internalResize(_capacity * 2); } \
            } \
            name1[_size] = n1; \
            name2[_size] = n2; \
            _size++; \
        } \
        inline void pop() \
        { \
            assert(_size > 0); \
            _size--; \
        } \
        void move(uint32_t dstIdx, uint32_t srcIdx) \
        { \
            assert(dstIdx >= 0 && dstIdx < _size && srcIdx >= 0 && srcIdx < _size); \
            name1[dstIdx] = name1[srcIdx]; \
            name2[dstIdx] = name2[srcIdx]; \
        } \
        void swap(uint32_t idx1, uint32_t idx2) \
        { \
            assert(idx1 >= 0 && idx1 < _size && idx2 >= 0 && idx2 < _size); \
            type1 t1 = name1[idx1]; name1[idx1] = name1[idx2]; name1[idx2] = t1; \
            type2 t2 = name2[idx1]; name2[idx1] = name2[idx2]; name2[idx2] = t2; \
        } \
        inline uint32_t getSize() { return _size; } \
        inline uint32_t capacity() { return _capacity; } \
        inline void setSize(uint32_t newSize) { assert(_size <= _capacity); _size = newSize; } \
        inline void resize(uint32_t newSize) \
        { \
            assert(newSize > 0); \
            internalResize(newSize); \
            _size = newSize; \
        } \
    }

#define CLASS_SOA_VECTOR3(className, type1, name1, type2, name2, type3, name3) \
    class className \
    { \
    private: \
        static_assert(alignof(type1) >= alignof(type2) && \
            alignof(type2) >= alignof(type3), \
            "Types must be listed in descending alignment."); \
        void* _memory; \
        uint32_t _size; \
        uint32_t _capacity; \
        void internalResize(uint32_t newCapacity) \
        { \
            const uint32_t TOTAL_ELEMENT_SIZE = sizeof(type1) + sizeof(type2) + sizeof(type3); \
            const uint32_t ALIGNMENT = alignof(type1); \
            void* newMemory = nullptr; \
            type1* newName1 = nullptr; \
            type2* newName2 = nullptr; \
            type3* newName3 = nullptr; \
            if (newCapacity > 0) \
            { \
                newMemory = _aligned_malloc(TOTAL_ELEMENT_SIZE * newCapacity, ALIGNMENT); \
                newName1 = (type1*)newMemory; \
                newName2 = (type2*)(newName1 + newCapacity); \
                newName3 = (type3*)(newName2 + newCapacity); \
            } \
            if (newCapacity < _size) { _size = newCapacity; } \
            if (_memory != nullptr && newMemory != nullptr) \
            { \
                memcpy(newName1, name1, sizeof(type1) * _size); \
                memcpy(newName2, name2, sizeof(type2) * _size); \
                memcpy(newName3, name3, sizeof(type3) * _size); \
            } \
            _aligned_free(_memory); \
            _memory = newMemory; \
            name1 = newName1; \
            name2 = newName2; \
            name3 = newName3; \
            _capacity = newCapacity; \
        } \
    public: \
        type1* name1; \
        type2* name2; \
        type3* name3; \
        className() : _memory(nullptr), _size(0), _capacity(0) { } \
        className(uint32_t capacity) : _memory(nullptr), _size(0), _capacity(0) \
        { \
            internalResize(capacity); \
        } \
        ~className() \
        { \
            _aligned_free(_memory); \
            _size = 0; \
            _capacity = 0; \
        } \
        void push(type1 n1, type2 n2, type3 n3) \
        { \
            if (_size == _capacity) \
            { \
                if (_capacity == 0) { internalResize(32); } \
                else { internalResize(_capacity * 2); } \
            } \
            name1[_size] = n1; \
            name2[_size] = n2; \
            name3[_size] = n3; \
            _size++; \
        } \
        inline void pop() \
        { \
            assert(_size > 0); \
            _size--; \
        } \
        void move(uint32_t dstIdx, uint32_t srcIdx) \
        { \
            assert(dstIdx >= 0 && dstIdx < _size && srcIdx >= 0 && srcIdx < _size); \
            name1[dstIdx] = name1[srcIdx]; \
            name2[dstIdx] = name2[srcIdx]; \
            name3[dstIdx] = name3[srcIdx]; \
        } \
        void swap(uint32_t idx1, uint32_t idx2) \
        { \
            assert(idx1 >= 0 && idx1 < _size && idx2 >= 0 && idx2 < _size); \
            type1 t1 = name1[idx1]; name1[idx1] = name1[idx2]; name1[idx2] = t1; \
            type2 t2 = name2[idx1]; name2[idx1] = name2[idx2]; name2[idx2] = t2; \
            type3 t3 = name3[idx1]; name3[idx1] = name3[idx2]; name3[idx2] = t3; \
        } \
        inline uint32_t getSize() { return _size; } \
        inline uint32_t capacity() { return _capacity; } \
        inline void setSize(uint32_t newSize) { assert(_size <= _capacity); _size = newSize; } \
        inline void resize(uint32_t newSize) \
        { \
            assert(newSize > 0); \
            internalResize(newSize); \
            _size = newSize; \
        } \
    }

#define CLASS_SOA_VECTOR4(className, type1, name1, type2, name2, type3, name3, type4, name4) \
    class className \
    { \
    private: \
        static_assert(alignof(type1) >= alignof(type2) && \
            alignof(type2) >= alignof(type3) && \
            alignof(type3) >= alignof(type4), \
            "Types must be listed in descending alignment."); \
        void* _memory; \
        uint32_t _size; \
        uint32_t _capacity; \
        void internalResize(uint32_t newCapacity) \
        { \
            const uint32_t TOTAL_ELEMENT_SIZE = sizeof(type1) + sizeof(type2) + sizeof(type3) + sizeof(type4); \
            const uint32_t ALIGNMENT = alignof(type1); \
            void* newMemory = nullptr; \
            type1* newName1 = nullptr; \
            type2* newName2 = nullptr; \
            type3* newName3 = nullptr; \
            type4* newName4 = nullptr; \
            if (newCapacity > 0) \
            { \
                newMemory = _aligned_malloc(TOTAL_ELEMENT_SIZE * newCapacity, ALIGNMENT); \
                newName1 = (type1*)newMemory; \
                newName2 = (type2*)(newName1 + newCapacity); \
                newName3 = (type3*)(newName2 + newCapacity); \
                newName4 = (type4*)(newName3 + newCapacity); \
            } \
            if (newCapacity < _size) { _size = newCapacity; } \
            if (_memory != nullptr && newMemory != nullptr) \
            { \
                memcpy(newName1, name1, sizeof(type1) * _size); \
                memcpy(newName2, name2, sizeof(type2) * _size); \
                memcpy(newName3, name3, sizeof(type3) * _size); \
                memcpy(newName4, name4, sizeof(type4) * _size); \
            } \
            _aligned_free(_memory); \
            _memory = newMemory; \
            name1 = newName1; \
            name2 = newName2; \
            name3 = newName3; \
            name4 = newName4; \
            _capacity = newCapacity; \
        } \
    public: \
        type1* name1; \
        type2* name2; \
        type3* name3; \
        type4* name4; \
        className() : _memory(nullptr), _size(0), _capacity(0) { } \
        className(uint32_t capacity) : _memory(nullptr), _size(0), _capacity(0) \
        { \
            internalResize(capacity); \
        } \
        ~className() \
        { \
            _aligned_free(_memory); \
            _size = 0; \
            _capacity = 0; \
        } \
        void push(type1 n1, type2 n2, type3 n3, type4 n4) \
        { \
            if (_size == _capacity) \
            { \
                if (_capacity == 0) { internalResize(32); } \
                else { internalResize(_capacity * 2); } \
            } \
            name1[_size] = n1; \
            name2[_size] = n2; \
            name3[_size] = n3; \
            name4[_size] = n4; \
            _size++; \
        } \
        inline void pop() \
        { \
            assert(_size > 0); \
            _size--; \
        } \
        void move(uint32_t dstIdx, uint32_t srcIdx) \
        { \
            assert(dstIdx >= 0 && dstIdx < _size && srcIdx >= 0 && srcIdx < _size); \
            name1[dstIdx] = name1[srcIdx]; \
            name2[dstIdx] = name2[srcIdx]; \
            name3[dstIdx] = name3[srcIdx]; \
            name4[dstIdx] = name4[srcIdx]; \
        } \
        void swap(uint32_t idx1, uint32_t idx2) \
        { \
            assert(idx1 >= 0 && idx1 < _size && idx2 >= 0 && idx2 < _size); \
            type1 t1 = name1[idx1]; name1[idx1] = name1[idx2]; name1[idx2] = t1; \
            type2 t2 = name2[idx1]; name2[idx1] = name2[idx2]; name2[idx2] = t2; \
            type3 t3 = name3[idx1]; name3[idx1] = name3[idx2]; name3[idx2] = t3; \
            type4 t4 = name4[idx1]; name4[idx1] = name4[idx2]; name4[idx2] = t4; \
        } \
        inline uint32_t getSize() { return _size; } \
        inline uint32_t capacity() { return _capacity; } \
        inline void setSize(uint32_t newSize) { assert(_size <= _capacity); _size = newSize; } \
        inline void resize(uint32_t newSize) \
        { \
            assert(newSize > 0); \
            internalResize(newSize); \
            _size = newSize; \
        } \
    }

#define CLASS_SOA_VECTOR5(className, type1, name1, type2, name2, type3, name3, type4, name4, type5, name5) \
    class className \
    { \
    private: \
        static_assert(alignof(type1) >= alignof(type2) && \
            alignof(type2) >= alignof(type3) && \
            alignof(type3) >= alignof(type4) && \
            alignof(type4) >= alignof(type5), \
            "Types must be listed in descending alignment."); \
        void* _memory; \
        uint32_t _size; \
        uint32_t _capacity; \
        void internalResize(uint32_t newCapacity) \
        { \
            const uint32_t TOTAL_ELEMENT_SIZE = sizeof(type1) + sizeof(type2) + sizeof(type3) + sizeof(type4) + sizeof(type5); \
            const uint32_t ALIGNMENT = alignof(type1); \
            void* newMemory = nullptr; \
            type1* newName1 = nullptr; \
            type2* newName2 = nullptr; \
            type3* newName3 = nullptr; \
            type4* newName4 = nullptr; \
            type5* newName5 = nullptr; \
            if (newCapacity > 0) \
            { \
                newMemory = _aligned_malloc(TOTAL_ELEMENT_SIZE * newCapacity, ALIGNMENT); \
                newName1 = (type1*)newMemory; \
                newName2 = (type2*)(newName1 + newCapacity); \
                newName3 = (type3*)(newName2 + newCapacity); \
                newName4 = (type4*)(newName3 + newCapacity); \
                newName5 = (type5*)(newName4 + newCapacity); \
            } \
            if (newCapacity < _size) { _size = newCapacity; } \
            if (_memory != nullptr && newMemory != nullptr) \
            { \
                memcpy(newName1, name1, sizeof(type1) * _size); \
                memcpy(newName2, name2, sizeof(type2) * _size); \
                memcpy(newName3, name3, sizeof(type3) * _size); \
                memcpy(newName4, name4, sizeof(type4) * _size); \
                memcpy(newName5, name5, sizeof(type5) * _size); \
            } \
            _aligned_free(_memory); \
            _memory = newMemory; \
            name1 = newName1; \
            name2 = newName2; \
            name3 = newName3; \
            name4 = newName4; \
            name5 = newName5; \
            _capacity = newCapacity; \
        } \
    public: \
        type1* name1; \
        type2* name2; \
        type3* name3; \
        type4* name4; \
        type5* name5; \
        className() : _memory(nullptr), _size(0), _capacity(0) { } \
        className(uint32_t capacity) : _memory(nullptr), _size(0), _capacity(0) \
        { \
            internalResize(capacity); \
        } \
        ~className() \
        { \
            _aligned_free(_memory); \
            _size = 0; \
            _capacity = 0; \
        } \
        void push(type1 n1, type2 n2, type3 n3, type4 n4, type5 n5) \
        { \
            if (_size == _capacity) \
            { \
                if (_capacity == 0) { internalResize(32); } \
                else { internalResize(_capacity * 2); } \
            } \
            name1[_size] = n1; \
            name2[_size] = n2; \
            name3[_size] = n3; \
            name4[_size] = n4; \
            name5[_size] = n5; \
            _size++; \
        } \
        inline void pop() \
        { \
            assert(_size > 0); \
            _size--; \
        } \
        void move(uint32_t dstIdx, uint32_t srcIdx) \
        { \
            assert(dstIdx >= 0 && dstIdx < _size && srcIdx >= 0 && srcIdx < _size); \
            name1[dstIdx] = name1[srcIdx]; \
            name2[dstIdx] = name2[srcIdx]; \
            name3[dstIdx] = name3[srcIdx]; \
            name4[dstIdx] = name4[srcIdx]; \
            name5[dstIdx] = name5[srcIdx]; \
        } \
        void swap(uint32_t idx1, uint32_t idx2) \
        { \
            assert(idx1 >= 0 && idx1 < _size && idx2 >= 0 && idx2 < _size); \
            type1 t1 = name1[idx1]; name1[idx1] = name1[idx2]; name1[idx2] = t1; \
            type2 t2 = name2[idx1]; name2[idx1] = name2[idx2]; name2[idx2] = t2; \
            type3 t3 = name3[idx1]; name3[idx1] = name3[idx2]; name3[idx2] = t3; \
            type4 t4 = name4[idx1]; name4[idx1] = name4[idx2]; name4[idx2] = t4; \
            type5 t5 = name5[idx1]; name5[idx1] = name5[idx2]; name5[idx2] = t5; \
        } \
        inline uint32_t getSize() { return _size; } \
        inline uint32_t capacity() { return _capacity; } \
        inline void setSize(uint32_t newSize) { assert(_size <= _capacity); _size = newSize; } \
        inline void resize(uint32_t newSize) \
        { \
            assert(newSize > 0); \
            internalResize(newSize); \
            _size = newSize; \
        } \
    }

#define CLASS_SOA_VECTOR6(className, type1, name1, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6) \
    class className \
    { \
    private: \
        static_assert(alignof(type1) >= alignof(type2) && \
            alignof(type2) >= alignof(type3) && \
            alignof(type3) >= alignof(type4) && \
            alignof(type4) >= alignof(type5) && \
            alignof(type5) >= alignof(type6), \
            "Types must be listed in descending alignment."); \
        void* _memory; \
        uint32_t _size; \
        uint32_t _capacity; \
        void internalResize(uint32_t newCapacity) \
        { \
            const uint32_t TOTAL_ELEMENT_SIZE = sizeof(type1) + sizeof(type2) + sizeof(type3) + sizeof(type4) + sizeof(type5) + sizeof(type6); \
            const uint32_t ALIGNMENT = alignof(type1); \
            void* newMemory = nullptr; \
            type1* newName1 = nullptr; \
            type2* newName2 = nullptr; \
            type3* newName3 = nullptr; \
            type4* newName4 = nullptr; \
            type5* newName5 = nullptr; \
            type6* newName6 = nullptr; \
            if (newCapacity > 0) \
            { \
                newMemory = _aligned_malloc(TOTAL_ELEMENT_SIZE * newCapacity, ALIGNMENT); \
                newName1 = (type1*)newMemory; \
                newName2 = (type2*)(newName1 + newCapacity); \
                newName3 = (type3*)(newName2 + newCapacity); \
                newName4 = (type4*)(newName3 + newCapacity); \
                newName5 = (type5*)(newName4 + newCapacity); \
                newName6 = (type6*)(newName5 + newCapacity); \
            } \
            if (newCapacity < _size) { _size = newCapacity; } \
            if (_memory != nullptr && newMemory != nullptr) \
            { \
                memcpy(newName1, name1, sizeof(type1) * _size); \
                memcpy(newName2, name2, sizeof(type2) * _size); \
                memcpy(newName3, name3, sizeof(type3) * _size); \
                memcpy(newName4, name4, sizeof(type4) * _size); \
                memcpy(newName5, name5, sizeof(type5) * _size); \
                memcpy(newName6, name6, sizeof(type6) * _size); \
            } \
            _aligned_free(_memory); \
            _memory = newMemory; \
            name1 = newName1; \
            name2 = newName2; \
            name3 = newName3; \
            name4 = newName4; \
            name5 = newName5; \
            name6 = newName6; \
            _capacity = newCapacity; \
        } \
    public: \
        type1* name1; \
        type2* name2; \
        type3* name3; \
        type4* name4; \
        type5* name5; \
        type6* name6; \
        className() : _memory(nullptr), _size(0), _capacity(0) { } \
        className(uint32_t capacity) : _memory(nullptr), _size(0), _capacity(0) \
        { \
            internalResize(capacity); \
        } \
        ~className() \
        { \
            _aligned_free(_memory); \
            _size = 0; \
            _capacity = 0; \
        } \
        void push(type1 n1, type2 n2, type3 n3, type4 n4, type5 n5, type6 n6) \
        { \
            if (_size == _capacity) \
            { \
                if (_capacity == 0) { internalResize(32); } \
                else { internalResize(_capacity * 2); } \
            } \
            name1[_size] = n1; \
            name2[_size] = n2; \
            name3[_size] = n3; \
            name4[_size] = n4; \
            name5[_size] = n5; \
            name6[_size] = n6; \
            _size++; \
        } \
        inline void pop() \
        { \
            assert(_size > 0); \
            _size--; \
        } \
        void move(uint32_t dstIdx, uint32_t srcIdx) \
        { \
            assert(dstIdx >= 0 && dstIdx < _size && srcIdx >= 0 && srcIdx < _size); \
            name1[dstIdx] = name1[srcIdx]; \
            name2[dstIdx] = name2[srcIdx]; \
            name3[dstIdx] = name3[srcIdx]; \
            name4[dstIdx] = name4[srcIdx]; \
            name5[dstIdx] = name5[srcIdx]; \
            name6[dstIdx] = name6[srcIdx]; \
        } \
        void swap(uint32_t idx1, uint32_t idx2) \
        { \
            assert(idx1 >= 0 && idx1 < _size && idx2 >= 0 && idx2 < _size); \
            type1 t1 = name1[idx1]; name1[idx1] = name1[idx2]; name1[idx2] = t1; \
            type2 t2 = name2[idx1]; name2[idx1] = name2[idx2]; name2[idx2] = t2; \
            type3 t3 = name3[idx1]; name3[idx1] = name3[idx2]; name3[idx2] = t3; \
            type4 t4 = name4[idx1]; name4[idx1] = name4[idx2]; name4[idx2] = t4; \
            type5 t5 = name5[idx1]; name5[idx1] = name5[idx2]; name5[idx2] = t5; \
            type6 t6 = name6[idx1]; name6[idx1] = name6[idx2]; name6[idx2] = t6; \
        } \
        inline uint32_t getSize() { return _size; } \
        inline uint32_t capacity() { return _capacity; } \
        inline void setSize(uint32_t newSize) { assert(_size <= _capacity); _size = newSize; } \
        inline void resize(uint32_t newSize) \
        { \
            assert(newSize > 0); \
            internalResize(newSize); \
            _size = newSize; \
        } \
    }

#define CLASS_SOA_VECTOR7(className, type1, name1, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7) \
    class className \
    { \
    private: \
        static_assert(alignof(type1) >= alignof(type2) && \
            alignof(type2) >= alignof(type3) && \
            alignof(type3) >= alignof(type4) && \
            alignof(type4) >= alignof(type5) && \
            alignof(type5) >= alignof(type6) && \
            alignof(type6) >= alignof(type7), \
            "Types must be listed in descending alignment."); \
        void* _memory; \
        uint32_t _size; \
        uint32_t _capacity; \
        void internalResize(uint32_t newCapacity) \
        { \
            const uint32_t TOTAL_ELEMENT_SIZE = sizeof(type1) + sizeof(type2) + sizeof(type3) + sizeof(type4) + sizeof(type5) + sizeof(type6) + sizeof(type7); \
            const uint32_t ALIGNMENT = alignof(type1); \
            void* newMemory = nullptr; \
            type1* newName1 = nullptr; \
            type2* newName2 = nullptr; \
            type3* newName3 = nullptr; \
            type4* newName4 = nullptr; \
            type5* newName5 = nullptr; \
            type6* newName6 = nullptr; \
            type7* newName7 = nullptr; \
            if (newCapacity > 0) \
            { \
                newMemory = _aligned_malloc(TOTAL_ELEMENT_SIZE * newCapacity, ALIGNMENT); \
                newName1 = (type1*)newMemory; \
                newName2 = (type2*)(newName1 + newCapacity); \
                newName3 = (type3*)(newName2 + newCapacity); \
                newName4 = (type4*)(newName3 + newCapacity); \
                newName5 = (type5*)(newName4 + newCapacity); \
                newName6 = (type6*)(newName5 + newCapacity); \
                newName7 = (type7*)(newName6 + newCapacity); \
            } \
            if (newCapacity < _size) { _size = newCapacity; } \
            if (_memory != nullptr && newMemory != nullptr) \
            { \
                memcpy(newName1, name1, sizeof(type1) * _size); \
                memcpy(newName2, name2, sizeof(type2) * _size); \
                memcpy(newName3, name3, sizeof(type3) * _size); \
                memcpy(newName4, name4, sizeof(type4) * _size); \
                memcpy(newName5, name5, sizeof(type5) * _size); \
                memcpy(newName6, name6, sizeof(type6) * _size); \
                memcpy(newName7, name7, sizeof(type7) * _size); \
            } \
            _aligned_free(_memory); \
            _memory = newMemory; \
            name1 = newName1; \
            name2 = newName2; \
            name3 = newName3; \
            name4 = newName4; \
            name5 = newName5; \
            name6 = newName6; \
            name7 = newName7; \
            _capacity = newCapacity; \
        } \
    public: \
        type1* name1; \
        type2* name2; \
        type3* name3; \
        type4* name4; \
        type5* name5; \
        type6* name6; \
        type7* name7; \
        className() : _memory(nullptr), _size(0), _capacity(0) { } \
        className(uint32_t capacity) : _memory(nullptr), _size(0), _capacity(0) \
        { \
            internalResize(capacity); \
        } \
        ~className() \
        { \
            _aligned_free(_memory); \
            _size = 0; \
            _capacity = 0; \
        } \
        void push(type1 n1, type2 n2, type3 n3, type4 n4, type5 n5, type6 n6, type7 n7) \
        { \
            if (_size == _capacity) \
            { \
                if (_capacity == 0) { internalResize(32); } \
                else { internalResize(_capacity * 2); } \
            } \
            name1[_size] = n1; \
            name2[_size] = n2; \
            name3[_size] = n3; \
            name4[_size] = n4; \
            name5[_size] = n5; \
            name6[_size] = n6; \
            name7[_size] = n7; \
            _size++; \
        } \
        inline void pop() \
        { \
            assert(_size > 0); \
            _size--; \
        } \
        void move(uint32_t dstIdx, uint32_t srcIdx) \
        { \
            assert(dstIdx >= 0 && dstIdx < _size && srcIdx >= 0 && srcIdx < _size); \
            name1[dstIdx] = name1[srcIdx]; \
            name2[dstIdx] = name2[srcIdx]; \
            name3[dstIdx] = name3[srcIdx]; \
            name4[dstIdx] = name4[srcIdx]; \
            name5[dstIdx] = name5[srcIdx]; \
            name6[dstIdx] = name6[srcIdx]; \
            name7[dstIdx] = name7[srcIdx]; \
        } \
        void swap(uint32_t idx1, uint32_t idx2) \
        { \
            assert(idx1 >= 0 && idx1 < _size && idx2 >= 0 && idx2 < _size); \
            type1 t1 = name1[idx1]; name1[idx1] = name1[idx2]; name1[idx2] = t1; \
            type2 t2 = name2[idx1]; name2[idx1] = name2[idx2]; name2[idx2] = t2; \
            type3 t3 = name3[idx1]; name3[idx1] = name3[idx2]; name3[idx2] = t3; \
            type4 t4 = name4[idx1]; name4[idx1] = name4[idx2]; name4[idx2] = t4; \
            type5 t5 = name5[idx1]; name5[idx1] = name5[idx2]; name5[idx2] = t5; \
            type6 t6 = name6[idx1]; name6[idx1] = name6[idx2]; name6[idx2] = t6; \
            type7 t7 = name7[idx1]; name7[idx1] = name7[idx2]; name7[idx2] = t7; \
        } \
        inline uint32_t getSize() { return _size; } \
        inline uint32_t capacity() { return _capacity; } \
        inline void setSize(uint32_t newSize) { assert(_size <= _capacity); _size = newSize; } \
        inline void resize(uint32_t newSize) \
        { \
            assert(newSize > 0); \
            internalResize(newSize); \
            _size = newSize; \
        } \
    }

#define CLASS_SOA_VECTOR8(className, type1, name1, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8) \
    class className \
    { \
    private: \
        static_assert(alignof(type1) >= alignof(type2) && \
            alignof(type2) >= alignof(type3) && \
            alignof(type3) >= alignof(type4) && \
            alignof(type4) >= alignof(type5) && \
            alignof(type5) >= alignof(type6) && \
            alignof(type6) >= alignof(type7) && \
            alignof(type7) >= alignof(type8), \
            "Types must be listed in descending alignment."); \
        void* _memory; \
        uint32_t _size; \
        uint32_t _capacity; \
        void internalResize(uint32_t newCapacity) \
        { \
            const uint32_t TOTAL_ELEMENT_SIZE = sizeof(type1) + sizeof(type2) + sizeof(type3) + sizeof(type4) + sizeof(type5) + sizeof(type6) + sizeof(type7) + sizeof(type8); \
            const uint32_t ALIGNMENT = alignof(type1); \
            void* newMemory = nullptr; \
            type1* newName1 = nullptr; \
            type2* newName2 = nullptr; \
            type3* newName3 = nullptr; \
            type4* newName4 = nullptr; \
            type5* newName5 = nullptr; \
            type6* newName6 = nullptr; \
            type7* newName7 = nullptr; \
            type8* newName8 = nullptr; \
            if (newCapacity > 0) \
            { \
                newMemory = _aligned_malloc(TOTAL_ELEMENT_SIZE * newCapacity, ALIGNMENT); \
                newName1 = (type1*)newMemory; \
                newName2 = (type2*)(newName1 + newCapacity); \
                newName3 = (type3*)(newName2 + newCapacity); \
                newName4 = (type4*)(newName3 + newCapacity); \
                newName5 = (type5*)(newName4 + newCapacity); \
                newName6 = (type6*)(newName5 + newCapacity); \
                newName7 = (type7*)(newName6 + newCapacity); \
                newName8 = (type8*)(newName7 + newCapacity); \
            } \
            if (newCapacity < _size) { _size = newCapacity; } \
            if (_memory != nullptr && newMemory != nullptr) \
            { \
                memcpy(newName1, name1, sizeof(type1) * _size); \
                memcpy(newName2, name2, sizeof(type2) * _size); \
                memcpy(newName3, name3, sizeof(type3) * _size); \
                memcpy(newName4, name4, sizeof(type4) * _size); \
                memcpy(newName5, name5, sizeof(type5) * _size); \
                memcpy(newName6, name6, sizeof(type6) * _size); \
                memcpy(newName7, name7, sizeof(type7) * _size); \
                memcpy(newName8, name8, sizeof(type8) * _size); \
            } \
            _aligned_free(_memory); \
            _memory = newMemory; \
            name1 = newName1; \
            name2 = newName2; \
            name3 = newName3; \
            name4 = newName4; \
            name5 = newName5; \
            name6 = newName6; \
            name7 = newName7; \
            name8 = newName8; \
            _capacity = newCapacity; \
        } \
    public: \
        type1* name1; \
        type2* name2; \
        type3* name3; \
        type4* name4; \
        type5* name5; \
        type6* name6; \
        type7* name7; \
        type8* name8; \
        className() : _memory(nullptr), _size(0), _capacity(0) { } \
        className(uint32_t capacity) : _memory(nullptr), _size(0), _capacity(0) \
        { \
            internalResize(capacity); \
        } \
        ~className() \
        { \
            _aligned_free(_memory); \
            _size = 0; \
            _capacity = 0; \
        } \
        void push(type1 n1, type2 n2, type3 n3, type4 n4, type5 n5, type6 n6, type7 n7, type8 n8) \
        { \
            if (_size == _capacity) \
            { \
                if (_capacity == 0) { internalResize(32); } \
                else { internalResize(_capacity * 2); } \
            } \
            name1[_size] = n1; \
            name2[_size] = n2; \
            name3[_size] = n3; \
            name4[_size] = n4; \
            name5[_size] = n5; \
            name6[_size] = n6; \
            name7[_size] = n7; \
            name8[_size] = n8; \
            _size++; \
        } \
        inline void pop() \
        { \
            assert(_size > 0); \
            _size--; \
        } \
        void move(uint32_t dstIdx, uint32_t srcIdx) \
        { \
            assert(dstIdx >= 0 && dstIdx < _size && srcIdx >= 0 && srcIdx < _size); \
            name1[dstIdx] = name1[srcIdx]; \
            name2[dstIdx] = name2[srcIdx]; \
            name3[dstIdx] = name3[srcIdx]; \
            name4[dstIdx] = name4[srcIdx]; \
            name5[dstIdx] = name5[srcIdx]; \
            name6[dstIdx] = name6[srcIdx]; \
            name7[dstIdx] = name7[srcIdx]; \
            name8[dstIdx] = name8[srcIdx]; \
        } \
        void swap(uint32_t idx1, uint32_t idx2) \
        { \
            assert(idx1 >= 0 && idx1 < _size && idx2 >= 0 && idx2 < _size); \
            type1 t1 = name1[idx1]; name1[idx1] = name1[idx2]; name1[idx2] = t1; \
            type2 t2 = name2[idx1]; name2[idx1] = name2[idx2]; name2[idx2] = t2; \
            type3 t3 = name3[idx1]; name3[idx1] = name3[idx2]; name3[idx2] = t3; \
            type4 t4 = name4[idx1]; name4[idx1] = name4[idx2]; name4[idx2] = t4; \
            type5 t5 = name5[idx1]; name5[idx1] = name5[idx2]; name5[idx2] = t5; \
            type6 t6 = name6[idx1]; name6[idx1] = name6[idx2]; name6[idx2] = t6; \
            type7 t7 = name7[idx1]; name7[idx1] = name7[idx2]; name7[idx2] = t7; \
            type8 t8 = name8[idx1]; name8[idx1] = name8[idx2]; name8[idx2] = t8; \
        } \
        inline uint32_t getSize() { return _size; } \
        inline uint32_t capacity() { return _capacity; } \
        inline void setSize(uint32_t newSize) { assert(_size <= _capacity); _size = newSize; } \
        inline void resize(uint32_t newSize) \
        { \
            assert(newSize > 0); \
            internalResize(newSize); \
            _size = newSize; \
        } \
    }

#endif
