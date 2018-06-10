#include "Core/Core.h"

#ifdef NW_ASSET_COOK
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <EASTL/hash_set.h>
#include <rapidjson/document.h>
#include "Cook.h"
#include "CookImpl.h"
#include "CookAsset.h"
#include "../Util/File.h"
#include "../Script/AngelState.h"   //For AngelState
#include "../AngelApplication.h"    //For registering application types

using namespace rapidjson;

namespace cook
{
    bool isCookable(const std::string& ext);

    uint32_t getHashSeed(const fs::path& assetFolder, const fs::path& cacheFolder);
    bool verifyHashSeed(const fs::path& assetFolder, uint32_t seed);
    uint32_t findHashSeed(const fs::path& assetFolder);
    uint32_t hashFile(const fs::path& path, uint32_t seed);

    fs::path relativeTo(fs::path from, fs::path to);



    void readCookSettings(const char* inputFile, CookSettings& output)
    {
        std::string json = file::readAllText(inputFile);
        Document jRoot;
        jRoot.Parse(json.c_str());
        output.assetFolder = jRoot["assetFolder"].GetString();
        output.cacheFolder = jRoot["cacheFolder"].GetString();
    }

    struct CookAssetArgs
    {
        FILE* assetNamesFile;           //File to write the asset names to (kinda hacky)
        const fs::path& assetFolder;    //Source folder for assets
        const fs::path& cacheFolder;    //Folder to cache cooked assets
        const fs::path& inFile;         //Path to file to cooked
        script::AngelState& angelState; //AngelState used for cooking
        uint32_t seed;                  //Seed used to hash file names
        CookAssetArgs(
            FILE* assetNamesFile,
            const fs::path& assetFolder,
            const fs::path& cacheFolder,
            const fs::path& inFile,
            script::AngelState& angelState,
            uint32_t seed) :
            assetNamesFile(assetNamesFile),
            assetFolder(assetFolder),
            cacheFolder(cacheFolder),
            inFile(inFile),
            angelState(angelState),
            seed(seed)
        {
        }
    };

    enum AssetCookPasses
    {
        COOK_PASS_ASSET,
        COOK_PASS_SCRIPT,
        COOK_PASS_SCENE,

        COOK_PASS_MAX
    };

    template <int cookPass>
    void cookAsset(const CookAssetArgs& args)
    {
        static_assert(cookPass < COOK_PASS_MAX, "Invalid cook pass.");

        //Check the extension
        auto ext = args.inFile.extension().string();
        if (!isCookable(ext)) { return; } // Unknown extension. Let's bail.

        //Build the final output path
        fs::path relativePath = relativeTo(args.assetFolder, args.inFile);
        uint32_t fileHash = hashFile(relativePath, args.seed);
        fs::path outFile = args.cacheFolder / hashToPath(fileHash);

        AssetCookData cdat;
        cdat.hashSeed = args.seed;
        cdat.assetFolder = args.assetFolder.string();
        cdat.inFile = args.inFile.string();
        cdat.inAssetPath = relativePath.string();
        cdat.outFile = outFile.string();

        std::replace(cdat.inFile.begin(), cdat.inFile.end(), '\\', '/');
        std::replace(cdat.inAssetPath.begin(), cdat.inAssetPath.end(), '\\', '/');
        std::replace(cdat.outFile.begin(), cdat.outFile.end(), '\\', '/');

        const char* in = cdat.inFile.c_str();
        const char* out = cdat.outFile.c_str();

        //Add to asset names file
        fprintf(args.assetNamesFile, "%08x %s\n", fileHash, cdat.inAssetPath.c_str());

        //Let's bail if outFile is newer than inFile
        if (cookPass == COOK_PASS_ASSET &&  //Always cook scripts and scenes
            fs::exists(outFile) &&
            fs::last_write_time(outFile) > fs::last_write_time(args.inFile))
        {
            printf("Skip: %s -> %s\n", in, out);
            return;
        }


        //Cook the asset based on its extension
        bool cooked = false;
        AssetFileWriter writer;
        cdat.writer = &writer;
        if (cookPass == COOK_PASS_ASSET)
        {
            if (ext == ".dds") { cookTexture2D(in, writer); cooked = true; }
            else if (ext == ".cvs" || ext == ".cps") { cookShader(in, writer); cooked = true; }
            else if (ext == ".wav") { cookSound(in, writer); cooked = true; }
            else if (ext == ".ogg") { cookMusic(in, writer); cooked = true; }
        }
        else if (cookPass == COOK_PASS_SCRIPT)
        {
            if (ext == ".as") { cookAngelScript(cdat, args.angelState); cooked = true; }
        }
        else if (cookPass == COOK_PASS_SCENE)
        {
            if (ext == ".scene") { cookScene(cdat, args.angelState); cooked = true; }
        }
        else
        {
            NW_ASSERT(false);
        }

        if (cooked)
        {
            printf("Cook: %s -> %s\n", in, out);
        }

        writer.saveToFile(out);
    }

    void cookDirHelper(
        FILE* assetNamesFile,
        const fs::path& inFolder,
        const fs::path& outRoot,
        script::AngelState& angelState,
        uint32_t seed,
        void (*fn)(const CookAssetArgs&))
    {
        //Iterate all files recursively and call the function
        fs::recursive_directory_iterator endIter;
        for (fs::recursive_directory_iterator iter(inFolder); iter != endIter; iter++)
        {
            if (fs::is_regular_file(iter->status()))
            {
                auto file = iter->path();
                CookAssetArgs args(
                    assetNamesFile,
                    inFolder,
                    outRoot,
                    file,
                    angelState,
                    seed);
                fn(args);
            }
        }
    }

    void cookAssets(const CookSettings& settings)
    {
        fs::path inFolder(settings.assetFolder);
        fs::path outRoot(settings.cacheFolder);

        //Make sure it's a folder
        if (!fs::is_directory(inFolder)) { return; }

        //Make the needed directories
        fs::create_directories(outRoot);
        fs::create_directories(outRoot / fs::path("Meta"));

        uint32_t hashSeed = getHashSeed(inFolder, outRoot);

        FILE* assetNamesFile = fopen("Assets.cpknames", "wb");

        //Create an AngelState for cooking assets.
        script::AngelState angelState;
        angelState.init();
        static Application* nullApp = nullptr;
        angelApplication_RegisterTypes(angelState.getScriptEngine(), &nullApp);

        //Multiple passes for compiling files
        // 1. Generic assets
        // 2. Scripts (get compiled during cook time)
        // 3. Scenes (always cooked since dependency tracking would be too complicated)
        cookDirHelper(assetNamesFile, inFolder, outRoot, angelState, hashSeed, cookAsset<(int)COOK_PASS_ASSET>);
        angelState.startCompiling();
        cookDirHelper(assetNamesFile, inFolder, outRoot, angelState, hashSeed, cookAsset<(int)COOK_PASS_SCRIPT>);
        angelState.endCompiling();
        cookDirHelper(assetNamesFile, inFolder, outRoot, angelState, hashSeed, cookAsset<(int)COOK_PASS_SCENE>);

        fclose(assetNamesFile);
    }

    bool isCookable(const std::string& ext)
    {
        return (ext == ".dds" ||
            ext == ".cvs" ||
            ext == ".cps" ||
            ext == ".wav" ||
            ext == ".ogg" ||
            ext == ".scene" ||
            ext == ".as");
    }



    //  getHashSeed
    //Gets a valid hash seed for an asset folder. Checks the seed file first
    //to see if we can use the same seed (lets us keep all the cached cooked
    //files). If the old seed isn't valid, the old cached files are deleted
    //to force recompilation.
    uint32_t getHashSeed(const fs::path& assetFolder, const fs::path& cacheFolder)
    {
        uint32_t seed = 0;
        fs::path seedFile = getSeedFilePath(cacheFolder);

        //Load old hash seed from file
        if (fs::exists(seedFile))
        {
            seed = readSeedFile(seedFile);

            //Verify that the old seed still works
            if (verifyHashSeed(assetFolder, seed))
            {
                return seed;
            }
            else
            {
                //No good. Delete all cached files to force recompilation.
                fs::remove_all(cacheFolder);
            }
        }

        printf("Finding a new hash seed. This might take a while.\n");
        seed = findHashSeed(assetFolder);

        //Write the "seed" file
        FILE* file = fopen(seedFile.string().c_str(), "w");
        fwrite(&seed, sizeof(seed), 1, file);
        fclose(file);

        return seed;
    }

    //  verifyHashSeed
    //Makes sure that the hash seed is still valid for all the assets in the
    //folder. In the event of a collision we need to generate a new hash (not
    //handled here though).
    //
    //Returns true if there are no collisions.
    bool verifyHashSeed(const fs::path& assetFolder, uint32_t seed)
    {
        //We use a HashSet to keep track of what hashes have already occurred
        eastl::hash_set<uint32_t> hashes;

        fs::recursive_directory_iterator endIter;
        for (fs::recursive_directory_iterator iter(assetFolder); iter != endIter; iter++)
        {
            auto path = iter->path();
            auto extension = path.extension().string();

            //We only check files that are actually going to be cooked
            if (fs::is_regular_file(iter->status()) && isCookable(extension))
            {
                uint32_t hash = hashFile(relativeTo(assetFolder, path), seed);

                auto search = hashes.find(hash);
                if (search != hashes.end())
                {
                    //Hash collision
                    return false;
                }
                else
                {
                    hashes.insert(hash);
                }
            }
        }

        //No hash collisions; the old hash seed is still valid
        return true;
    }

    //  fastVerifyHashSeed
    //Fast alternative to verifyHashSeed. Doesn't access the filesystem. Requires
    //the user to cache the filenames.
    bool fastVerifyHashSeed(const std::vector<fs::path>& files, uint32_t seed)
    {
        //We use a HashSet to keep track of what hashes have already occurred
        eastl::hash_set<uint32_t> hashes;

        for (auto& file : files)
        {
            uint32_t hash = hashFile(file, seed);

            auto search = hashes.find(hash);
            if (search != hashes.end())
            {
                //Hash collision
                return false;
            }
            else
            {
                hashes.insert(hash);
            }
        }

        //No hash collisions; the old hash seed is still valid
        return true;
    }

    //  findHashSeed()
    //Returns a no collision hash seed for an asset folder.
    uint32_t findHashSeed(const fs::path& assetFolder)
    {
        //Let's cache the list of files
        //This is going to take a while
        std::vector<fs::path> files;
        fs::recursive_directory_iterator endIter;
        for (fs::recursive_directory_iterator iter(assetFolder); iter != endIter; iter++)
        {
            auto path = iter->path();
            auto extension = path.extension().string();

            //We only check files that are actually going to be cooked
            if (fs::is_regular_file(iter->status()) && isCookable(extension))
            {
                //We push back the relative path to avoid the expensive recalculation
                files.push_back(relativeTo(assetFolder, path));
            }
        }

        //Now let's start testing seeds
        uint32_t seed = 0;
        while (true)
        {
            if (fastVerifyHashSeed(files, seed))
            {
                break;
            }

            //Hope to whatever funky god you believe in that you never hit this
            if (seed == UINT32_MAX)
            {
                printf("This is scary. There are no valid hash seeds.\n");
                printf("Try renaming a file that you recently added.\n");
                exit(666);
            }

            seed++;
        }

        return seed;
    }

    //  hashFile()
    //Returns the 32bit hash of a file name for a specified seed.
    uint32_t hashFile(const fs::path& path, uint32_t seed)
    {
        auto str = path.string();
        std::replace(str.begin(), str.end(), '\\', '/');
        return XXH32(str.c_str(), str.length(), seed);
    }

    //  getSeedFilePath()
    //Returns the path to the seed file for a specified cache folder.
    fs::path getSeedFilePath(const fs::path& cacheFolder)
    {
        return cacheFolder / fs::path("Meta/Seed");
    }

    //  readSeedFile()
    //Returns the seed contained in the seed file.
    uint32_t readSeedFile(const fs::path& seedFilePath)
    {
        uint32_t seed;
        FILE* seedFile = fopen(seedFilePath.string().c_str(), "r");
        fread(&seed, sizeof(seed), 1, seedFile);
        fclose(seedFile);
        return seed;
    }

    //  hashToPath()
    //Converts a 32bit hash to a hex path.
    fs::path hashToPath(uint32_t hash)
    {
        std::stringstream stream;
        stream << std::setfill('0') << std::setw(sizeof(uint32_t) * 2) << std::hex << hash;
        return fs::path(stream.str());
    }

    fs::path relativeTo(fs::path from, fs::path to)
    {
        fs::path::const_iterator fromIter = from.begin();
        fs::path::const_iterator toIter = to.begin();

        //Loop through both while they match
        while (fromIter != from.end() && toIter != to.end() && (*toIter) == (*fromIter))
        {
            toIter++;
            fromIter++;
        }

        fs::path finalPath;
        while (fromIter != from.end())
        {
            finalPath /= "..";
            fromIter++;
        }

        while (toIter != to.end())
        {
            finalPath /= *toIter;
            toIter++;
        }

        return finalPath;
    }
}
#endif
