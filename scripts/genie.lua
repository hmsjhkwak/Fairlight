ROOT_DIR = path.getabsolute("..")
DEPEND_DIR = path.getabsolute("../Dependencies")

solution "Fairlight"
    configurations {
        "Debug",    -- Non-optimized
        "Develop",  -- Optimized with development features
        "Profile",  -- Release but with profiling enabled
        "Release",  -- Optimized with all development features stripped
    }

    platforms {
        "x32",
        "x64",
    }

    language "C++"
    startproject "Fairlight"

---------------
-- Toolchain --
---------------
BUILD_DIR = path.join(ROOT_DIR, ".build")
THIRD_PARTY_DIR = path.join(ROOT_DIR, "3rdparty")
dofile (path.join(DEPEND_DIR, "bx/scripts/toolchain.lua"))
if not toolchain(BUILD_DIR, THIRD_PARTY_DIR) then
	return -- no action specified
end

    flags {
        "NoIncrementalLink",
    }

    -------------------
    -- Target Suffix --
    -------------------
    configuration { "Debug" }
        targetsuffix "Debug"
    configuration { "Develop" }
        targetsuffix "Develop"
    configuration { "Profile" }
        targetsuffix "Profile"
    configuration { "Release" }
        targetsuffix ""

    configuration { "Develop or Profile or Release" }
        flags {
            "NoBufferSecurityCheck",
            "OptimizeSpeed",
        }
        defines {
            "NDEBUG",
        }

dofile "angelscript.lua"
dofile "eastl.lua"
dofile "lz4.lua"

dofile "fairlight.lua"
