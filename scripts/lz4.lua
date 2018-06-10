LZ4_DIR = path.join(DEPEND_DIR, "LZ4")

project "LZ4"
    uuid (os.uuid("LZ4"))
    kind "StaticLib"

    includedirs {
        path.join(LZ4_DIR, "include/lz4")
    }

    files {
        path.join(LZ4_DIR, "source/**"),
    }
