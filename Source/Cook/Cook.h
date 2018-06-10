#ifdef NW_ASSET_COOK

#ifndef COOK_COOK_H
#define COOK_COOK_H

namespace cook
{
    class AssetFileWriter;

    struct CookSettings
    {
        std::string assetFolder;
        std::string cacheFolder;
    };

    struct AssetCookData
    {
        std::string assetFolder;    //Path to asset folder
        std::string inFile;         //Path relative to working directory
        std::string inAssetPath;    //Path relative to asset folder
        std::string outFile;        //Path relative to working directory
        AssetFileWriter* writer;
        uint32_t hashSeed;
    };

    void readCookSettings(const char* inputFile, CookSettings& output);
    void cookAssets(const CookSettings& settings);
}

#endif

#endif
