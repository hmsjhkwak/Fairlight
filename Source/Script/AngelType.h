#ifndef SCRIPT_ANGEL_TYPE_H
#define SCRIPT_ANGEL_TYPE_H

#include <ctype.h>
#include <string.h>
#include <EASTL/functional.h>
#include "Core/xxhash/xxhash.h"

namespace script
{
    struct AngelType
    {
        static const uint32_t NAME_HASH_SEED = 0x837ed454;   //Some random number
        enum class Type : uint32_t
        {
            Void,
            Int8,
            Int16,
            Int32,
            Int64,
            UInt8,
            UInt16,
            UInt32,
            UInt64,
            Float32,
            Float64,
            Bool,
            CustomType,
        };
        Type type;
        uint32_t nameHash;

        AngelType() : type(Type::Void), nameHash(0) { }
        AngelType(Type inType)
        {
            if (inType <= Type::Bool)
            {
                type = inType;
            }
            else
            {
                type = Type::Void;
            }
            nameHash = 0;
        }
        AngelType(const char* name)
        {
            nameHash = 0;
            if (islower(name[0]))
            {
                if (strcmp(name, "void") == 0) { type = Type::Void; return; }
                else if (strcmp(name, "int8") == 0) { type = Type::Int8; return; }
                else if (strcmp(name, "int16") == 0) { type = Type::Int16; return; }
                else if (strcmp(name, "int") == 0) { type = Type::Int32; return; }
                else if (strcmp(name, "int32") == 0) { type = Type::Int32; return; }
                else if (strcmp(name, "int64") == 0) { type = Type::Int64; return; }
                else if (strcmp(name, "uint8") == 0) { type = Type::UInt8; return; }
                else if (strcmp(name, "uint16") == 0) { type = Type::UInt16; return; }
                else if (strcmp(name, "uint") == 0) { type = Type::UInt32; return; }
                else if (strcmp(name, "uint32") == 0) { type = Type::UInt32; return; }
                else if (strcmp(name, "uint64") == 0) { type = Type::UInt64; return; }
                else if (strcmp(name, "float") == 0) { type = Type::Float32; return; }
                else if (strcmp(name, "double") == 0) { type = Type::Float64; return; }
                else if (strcmp(name, "bool") == 0) { type = Type::Bool; return; }
            }

            type = Type::CustomType;
            nameHash = XXH32(name, strlen(name), NAME_HASH_SEED);
        }

        bool operator==(AngelType other) const { return type == other.type && nameHash == other.nameHash; }
        bool operator!=(AngelType other) const { return type != other.type && nameHash != other.nameHash; }

        template <typename Archive>
        void serialize(Archive& ar)
        {
            uint32_t& typeRef = *((uint32_t*)&type);
            ar.serializeU32(typeRef);
            ar.serializeU32(nameHash);  //Could make this only serialized if type is custom...
        }
    };
}

template <>
struct eastl::hash<script::AngelType>
{
    size_t operator()(script::AngelType val) const
    {
        return (size_t)val.type | (size_t)val.nameHash;
    }
};

#endif
