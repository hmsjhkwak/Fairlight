#ifndef UTIL_ARCHIVES_H
#define UTIL_ARCHIVES_H

#include <stdint.h>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <EASTL/vector.h>
#include <EASTL/hash_map.h>

#define AR_SERIALIZE_ARRAY(ar, arr, size, fn) \
    do { \
        auto* _ptr = (arr); \
        for (uint32_t _idx = 0; _idx < size; _idx++) { ar.fn(_ptr[_idx]); } \
    } while (0)

#define AR_SERIALIZE_ARRAY_CHAR(ar, ptr, size) AR_SERIALIZE_ARRAY(ar, ptr, size, serializeChar)

#define AR_SERIALIZE_ARRAY_U8(ar, ptr, size) AR_SERIALIZE_ARRAY(ar, ptr, size, serializeU8)
#define AR_SERIALIZE_ARRAY_U16(ar, ptr, size) AR_SERIALIZE_ARRAY(ar, ptr, size, serializeU16)
#define AR_SERIALIZE_ARRAY_U32(ar, ptr, size) AR_SERIALIZE_ARRAY(ar, ptr, size, serializeU32)
#define AR_SERIALIZE_ARRAY_U64(ar, ptr, size) AR_SERIALIZE_ARRAY(ar, ptr, size, serializeU64)

#define AR_SERIALIZE_ARRAY_I8(ar, ptr, size) AR_SERIALIZE_ARRAY(ar, ptr, size, serializeI8)
#define AR_SERIALIZE_ARRAY_I16(ar, ptr, size) AR_SERIALIZE_ARRAY(ar, ptr, size, serializeI16)
#define AR_SERIALIZE_ARRAY_I32(ar, ptr, size) AR_SERIALIZE_ARRAY(ar, ptr, size, serializeI32)
#define AR_SERIALIZE_ARRAY_I64(ar, ptr, size) AR_SERIALIZE_ARRAY(ar, ptr, size, serializeI64)

#define AR_SERIALIZE_ARRAY_F32(ar, ptr, size) AR_SERIALIZE_ARRAY(ar, ptr, size, serializeF32)
#define AR_SERIALIZE_ARRAY_F64(ar, ptr, size) AR_SERIALIZE_ARRAY(ar, ptr, size, serializeF64)

#define AR_SERIALIZE_ARRAY_CUSTOM(ar, ptr, size) AR_SERIALIZE_ARRAY(ar, ptr, size, serializeCustom)

#define AR_SERIALIZE_VECTOR(ar, vec, fn) \
    do { \
        uint32_t _size = (uint32_t)vec.size(); \
        ar.serializeU32(_size); \
        if (ar.IsReading) { vec.resize(_size); } \
        AR_SERIALIZE_ARRAY(ar, vec.data(), _size, fn); \
    } while (0)

#define AR_SERIALIZE_MAP(ar, map, fn1, fn2) \
    if (ar.IsReading) { \
        uint32_t _mapSize; \
        ar.serializeU32(_mapSize); \
        for (uint32_t i = 0; i < _mapSize; i++) { \
            decltype(map)::key_type _first; ar.fn1(_first); \
            decltype(map)::mapped_type _second; ar.fn2(_second); \
            map.insert(eastl::make_pair(_first, _second)); \
        } \
    } \
    else { \
        uint32_t _mapSize = (uint32_t)map.size(); \
        ar.serializeU32(_mapSize); \
        for (auto& _pair : map) { \
            decltype(map)::key_type first = _pair.first; ar.fn1(first); \
            decltype(map)::mapped_type second = _pair.second; ar.fn2(second); \
        } \
    }

namespace util
{
    class MemoryReadArchive
    {
    private:
        const char* _memoryStart;
        const char* _memoryEnd;
        const char* _currentPosition;

        NW_FORCEINLINE void serializeInternal(void* ptr, size_t length)
        {
            //Bounds check commented out to assist inlining
            //NW_ASSERT(_currentPosition + length <= _memoryEnd);
            memcpy(ptr, _currentPosition, length);
            _currentPosition += length;
        }

    public:
        static const bool IsReading = true;
        static const bool IsWriting = false;

        MemoryReadArchive() :
            _memoryStart(nullptr),
            _memoryEnd(nullptr),
            _currentPosition(nullptr)
        {
        }

        void init(const void* data, size_t length)
        {
            _memoryStart = (char*)data;
            _memoryEnd = _memoryStart + length;
            _currentPosition = _memoryStart;
        }

        const char* startPtr() const { return _memoryStart; }
        const char* endPtr() const { return _memoryEnd; }
        const char* currentPtr() const { return _currentPosition; }


        // === Start Archive Interface ===
        NW_FORCEINLINE void serializeChar(char& v) { serializeInternal(&v, sizeof(v)); }

        NW_FORCEINLINE void serializeU8(uint8_t& v) { serializeInternal(&v, sizeof(v)); }
        NW_FORCEINLINE void serializeU16(uint16_t& v) { serializeInternal(&v, sizeof(v)); }
        NW_FORCEINLINE void serializeU32(uint32_t& v) { serializeInternal(&v, sizeof(v)); }
        NW_FORCEINLINE void serializeU64(uint64_t& v) { serializeInternal(&v, sizeof(v)); }

        NW_FORCEINLINE void serializeI8(int8_t& v) { serializeInternal(&v, sizeof(v)); }
        NW_FORCEINLINE void serializeI16(int16_t& v) { serializeInternal(&v, sizeof(v)); }
        NW_FORCEINLINE void serializeI32(int32_t& v) { serializeInternal(&v, sizeof(v)); }
        NW_FORCEINLINE void serializeI64(int64_t& v) { serializeInternal(&v, sizeof(v)); }

        NW_FORCEINLINE void serializeF32(float& v) { serializeInternal(&v, sizeof(v)); }
        NW_FORCEINLINE void serializeF64(double& v) { serializeInternal(&v, sizeof(v)); }

        template <typename T>
        NW_FORCEINLINE void serializeCustom(T& value)
        {
            static_assert(!std::is_fundamental<T>(), "serializeCustom() should not be called for primitives");
            value.serialize<MemoryReadArchive>(*this);
        }
        // === End Archive Interface ===
    };

    class EndianVectorWriteArchive
    {
    private:
        eastl::vector<uint8_t> _data;
        bool _flipEndian;

    public:
        static const bool IsReading = false;
        static const bool IsWriting = true;

        EndianVectorWriteArchive() : _flipEndian(false)
        {
        }

        void setFlipEndian(bool flip) { _flipEndian = flip; }
        uint8_t* data() { return _data.data(); }
        size_t size() { return _data.size(); }


        // === Start Archive Interface ===
#define IMPLEMENT_SERIALIZE_FN(fnname, type)\
        NW_FORCEINLINE void fnname(type data)\
        {\
            union { type value; char bytes[sizeof(data)]; } temp;\
            temp.value = data;\
            if (!_flipEndian) { for (int i = 0; i < sizeof(data); i++) { _data.push_back(temp.bytes[i]); } }\
            else              { for (int i = 0; i < sizeof(data); i++) { _data.push_back(temp.bytes[sizeof(data) - i]); } }\
        }

        IMPLEMENT_SERIALIZE_FN(serializeChar, char);

        IMPLEMENT_SERIALIZE_FN(serializeU8, uint8_t);
        IMPLEMENT_SERIALIZE_FN(serializeU16, uint16_t);
        IMPLEMENT_SERIALIZE_FN(serializeU32, uint32_t);
        IMPLEMENT_SERIALIZE_FN(serializeU64, uint64_t);

        IMPLEMENT_SERIALIZE_FN(serializeI8, int8_t);
        IMPLEMENT_SERIALIZE_FN(serializeI16, int16_t);
        IMPLEMENT_SERIALIZE_FN(serializeI32, int32_t);
        IMPLEMENT_SERIALIZE_FN(serializeI64, int64_t);

        IMPLEMENT_SERIALIZE_FN(serializeF32, float);
        IMPLEMENT_SERIALIZE_FN(serializeF64, double);
#undef IMPLEMENT_SERIALIZE_FN

        template <typename T>
        NW_FORCEINLINE void serializeCustom(T& value)
        {
            value.serialize<EndianVectorWriteArchive>(*this);
        }
        // === End Archive Interface ===


        //Hack used for overwriting already serialized values.
        //Useful in cases where you need to serialize a value up front
        //and the value is dependent on the success of later operations.
        size_t getOffset() { return _data.size(); }

#define IMPLEMENT_SERIALIZE_FN(fnname, type)\
        NW_FORCEINLINE void fnname(size_t offset, type data)\
        {\
            union { type value; char bytes[sizeof(data)]; } temp;\
            temp.value = data;\
            if (!_flipEndian) { for (int i = 0; i < sizeof(data); i++) { _data[offset + i] = temp.bytes[i]; } }\
            else              { for (int i = 0; i < sizeof(data); i++) { _data[offset + i] = temp.bytes[sizeof(data) - i]; } }\
        }

        IMPLEMENT_SERIALIZE_FN(offsetSerializeChar, char);

        IMPLEMENT_SERIALIZE_FN(offsetSerializeU8, uint8_t);
        IMPLEMENT_SERIALIZE_FN(offsetSerializeU16, uint16_t);
        IMPLEMENT_SERIALIZE_FN(offsetSerializeU32, uint32_t);
        IMPLEMENT_SERIALIZE_FN(offsetSerializeU64, uint64_t);

        IMPLEMENT_SERIALIZE_FN(offsetSerializeI8, int8_t);
        IMPLEMENT_SERIALIZE_FN(offsetSerializeI16, int16_t);
        IMPLEMENT_SERIALIZE_FN(offsetSerializeI32, int32_t);
        IMPLEMENT_SERIALIZE_FN(offsetSerializeI64, int64_t);

        IMPLEMENT_SERIALIZE_FN(offsetSerializeF32, float);
        IMPLEMENT_SERIALIZE_FN(offsetSerializeF64, double);
#undef IMPLEMENT_SERIALIZE_FN
    };
}

#endif
