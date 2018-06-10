SDL2_DIR = path.join(DEPEND_DIR, "SDL2-2.0.8")
SDL2_MIXER_DIR = path.join(DEPEND_DIR, "SDL2_mixer-2.0.2")

project "Fairlight"
    uuid (os.uuid("Fairlight"))
    kind "ConsoleApp"

    debugdir "../"

    removeflags {
        "NoPCH"
    }

    pchheader "Core/Core.h"
    pchsource "../Source/Core/Core.cpp"

    includedirs {
        path.join(ROOT_DIR, "Source"),
        path.join(DEPEND_DIR, "EASTL/include"),
        path.join(DEPEND_DIR, "bx/include"),
        path.join(DEPEND_DIR, "bimg/include"),
        path.join(DEPEND_DIR, "bgfx/include"),
        path.join(DEPEND_DIR, "angelscript/include"),
        path.join(DEPEND_DIR, "LuaJIT-2.0.4/src"),
        path.join(DEPEND_DIR, "lz4/include"),
        path.join(DEPEND_DIR, "rapidjson/include"),
        path.join(SDL2_DIR, "include"),
        path.join(SDL2_MIXER_DIR, "include"),
    }

    files {
        path.join(ROOT_DIR, "Source/**"),
    }

    nopch {
        path.join(ROOT_DIR, "Source/**.c"),
    }

    links {
        "angelscript",
        "EASTL",
        "LZ4",
        "SDL2",
        "SDL2main",
        "SDL2_mixer",
    }

    defines {
        "AS_NO_EXCEPTIONS",
        "EA_COMPILER_NO_NOEXCEPT",
    }

    configuration { "x32", "vs*" }
        includedirs {
            path.join(DEPEND_DIR, "bx/include/compat/msvc"),
        }
        libdirs {
            path.join(DEPEND_DIR, "bgfx/.build/win32_" .. _ACTION .. "/bin"),
            path.join(SDL2_DIR, "lib/x86"),
            path.join(SDL2_MIXER_DIR, "lib/x86"),
        }
        links {
            "psapi"
        }
    configuration { "x64", "vs*" }
        includedirs {
            path.join(DEPEND_DIR, "bx/include/compat/msvc"),
            path.join(DEPEND_DIR, "PIX/include"),
        }
        libdirs {
            path.join(DEPEND_DIR, "bgfx/.build/win64_" .. _ACTION .. "/bin"),
            path.join(SDL2_DIR, "lib/x64"),
            path.join(SDL2_MIXER_DIR, "lib/x64"),
            path.join(DEPEND_DIR, "PIX/bin"),
        }
        links {
            "WinPixEventRuntime",
        }

    -----------------------
    -- Include BGFX libs --
    -----------------------
    configuration { "Debug" }
        links {
            "bgfxDebug",
            "bimg_decodeDebug",
            "bimgDebug",
            "bxDebug",
        }
    configuration { "Develop or Profile or Release" }
        links {
            "bgfxRelease",
            "bimg_decodeRelease",
            "bimgRelease",
            "bxRelease",
        }

    ---------------------------
    -- Configuration Defines --
    ---------------------------
    configuration { "Debug" }
        defines {
            "NW_CONFIG_DEBUG",
            "NW_PROFILE",
        }

    configuration { "Develop" }
        defines {
            "NW_CONFIG_DEVELOP",
            "NW_PROFILE",
        }

    configuration { "Profile" }
        defines {
            "NW_CONFIG_RELEASE",
            "NW_PROFILE",
        }

    configuration { "Release" }
        defines {
            "NW_CONFIG_RELEASE",
        }

    -----------------------
    -- Compiler Settings --
    -----------------------
    configuration { "vs*" }
        buildoptions {
            "/wd4127",  -- warning C4127: conditional expression is constant
        }


    ---------------------
    -- Post Build Step --
    ---------------------
    configuration { "x32", "vs*" }
        postbuildcommands {
            -- Copy the DLLs
            "copy \"" ..
                path.translate(path.join(SDL2_DIR, "lib/x86/SDL2.dll")) .. "\" \"" ..
                path.translate(path.join(ROOT_DIR, ".build/win32_" .. _ACTION .. "/bin/SDL2.dll\"")),
            "copy \"" ..
                path.translate(path.join(SDL2_MIXER_DIR, "lib/x86/SDL2_mixer.dll")) .. "\" \"" ..
                path.translate(path.join(ROOT_DIR, ".build/win32_" .. _ACTION .. "/bin/SDL2_mixer.dll\"")),
            "copy \"" ..
                path.translate(path.join(SDL2_MIXER_DIR, "lib/x86/libogg-0.dll")) .. "\" \"" ..
                path.translate(path.join(ROOT_DIR, ".build/win32_" .. _ACTION .. "/bin/libogg-0.dll\"")),
            "copy \"" ..
                path.translate(path.join(SDL2_MIXER_DIR, "lib/x86/libvorbis-0.dll")) .. "\" \"" ..
                path.translate(path.join(ROOT_DIR, ".build/win32_" .. _ACTION .. "/bin/libvorbis-0.dll\"")),
            "copy \"" ..
                path.translate(path.join(SDL2_MIXER_DIR, "lib/x86/libvorbisfile-3.dll")) .. "\" \"" ..
                path.translate(path.join(ROOT_DIR, ".build/win32_" .. _ACTION .. "/bin/libvorbisfile-3.dll\"")),
            "copy \"" ..
                path.translate(path.join(DEPEND_DIR, "PIX/bin/WinPixEventRuntime.dll")) .. "\" \"" ..
                path.translate(path.join(ROOT_DIR, ".build/win32_" .. _ACTION .. "/bin/WinPixEventRuntime.dll\"")),
        }
    configuration { "x64", "vs*" }
        postbuildcommands {
            -- Copy the DLLs
            "copy \"" ..
                path.translate(path.join(SDL2_DIR, "lib/x64/SDL2.dll")) .. "\" \"" ..
                path.translate(path.join(ROOT_DIR, ".build/win64_" .. _ACTION .. "/bin/SDL2.dll\"")),
            "copy \"" ..
                path.translate(path.join(SDL2_MIXER_DIR, "lib/x64/SDL2_mixer.dll")) .. "\" \"" ..
                path.translate(path.join(ROOT_DIR, ".build/win64_" .. _ACTION .. "/bin/SDL2_mixer.dll\"")),
            "copy \"" ..
                path.translate(path.join(SDL2_MIXER_DIR, "lib/x64/libogg-0.dll")) .. "\" \"" ..
                path.translate(path.join(ROOT_DIR, ".build/win64_" .. _ACTION .. "/bin/libogg-0.dll\"")),
            "copy \"" ..
                path.translate(path.join(SDL2_MIXER_DIR, "lib/x64/libvorbis-0.dll")) .. "\" \"" ..
                path.translate(path.join(ROOT_DIR, ".build/win64_" .. _ACTION .. "/bin/libvorbis-0.dll\"")),
            "copy \"" ..
                path.translate(path.join(SDL2_MIXER_DIR, "lib/x64/libvorbisfile-3.dll")) .. "\" \"" ..
                path.translate(path.join(ROOT_DIR, ".build/win64_" .. _ACTION .. "/bin/libvorbisfile-3.dll\"")),
            "copy \"" ..
                path.translate(path.join(DEPEND_DIR, "PIX/bin/WinPixEventRuntime.dll")) .. "\" \"" ..
                path.translate(path.join(ROOT_DIR, ".build/win64_" .. _ACTION .. "/bin/WinPixEventRuntime.dll\"")),
        }
