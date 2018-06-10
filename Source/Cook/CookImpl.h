#ifdef NW_ASSET_COOK

#ifndef COOK_COOK_IMPL_H
#define COOK_COOK_IMPL_H

#include <filesystem>

//Wow, this almost makes me not want to stab my eyes out!
namespace fs { using namespace std::tr2::sys; }

namespace cook
{
    fs::path hashToPath(uint32_t hash);

    fs::path getSeedFilePath(const fs::path& cacheFolder);
    uint32_t readSeedFile(const fs::path& cacheFolder);
}

#endif

#endif
