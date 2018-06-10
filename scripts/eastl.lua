EASTL_DIR = path.join(DEPEND_DIR, "EASTL")

project "EASTL"
    uuid (os.uuid("EASTL"))
    kind "StaticLib"

    includedirs {
        path.join(EASTL_DIR, "include")
    }

    files {
        path.join(EASTL_DIR, "EASTL.natvis"),
        path.join(EASTL_DIR, "include/**"),
        path.join(EASTL_DIR, "source/**"),
    }

    defines {
        "EA_COMPILER_NO_NOEXCEPT",
    }
