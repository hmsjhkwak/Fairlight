#include "Core/Core.h"
#include <EASTL/hash_map.h>
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <sstream>
#include <iostream>
#include <string>
#include "AssetRef.h"
#include "Util/File.h"

namespace asset
{
    namespace AssetNameTable
    {
        struct NameTable
        {
            eastl::hash_map<uint32_t, uint32_t> hashToIndex;
            eastl::vector<eastl::string> names;
        };
        static NameTable g_NameTable;

        void loadAssetNames(const char* filename)
        {
            g_NameTable.names.push_back(eastl::string("~~~ Unknown Asset Name ~~~"));

            std::string contents = util::file::readAllText(filename);
            if (contents.empty()) { return; }
            std::stringstream input(contents);
            std::string line;
            while (std::getline(input, line))
            {
                std::string hexString = line.substr(0, sizeof(uint32_t) * 2);   //2 hex characters per byte
                if (hexString.length() != 8) { continue; }
                uint32_t hash = std::stoul(hexString, nullptr, 16);
                std::string name = line.substr(
                    hexString.length() + 1,
                    line.length() - hexString.length() - 1);

                g_NameTable.hashToIndex.insert(eastl::make_pair(hash, (uint32_t)g_NameTable.names.size()));
                g_NameTable.names.push_back(eastl::string(name.c_str()));
            }
        }

        uint32_t getIndex(uint32_t hash)
        {
            uint32_t index = 0;
            auto result = g_NameTable.hashToIndex.find(hash);
            if (result != g_NameTable.hashToIndex.end())
            {
                index = result->second;
            }
            return index;
        }

        const char* getName(uint32_t index)
        {
            return g_NameTable.names[index].c_str();
        }
    }
}
