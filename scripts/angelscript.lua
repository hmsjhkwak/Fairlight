ANGELSCRIPT_DIR = path.join(DEPEND_DIR, "angelscript")

project "angelscript"
    uuid (os.uuid("angelscript"))
    kind "StaticLib"

    includedirs {
        path.join(ANGELSCRIPT_DIR, "include")
    }

    files {
        path.join(ANGELSCRIPT_DIR, "source/*.h"),
        path.join(ANGELSCRIPT_DIR, "source/*.cpp"),
        path.join(ANGELSCRIPT_DIR, "source/scriptbuilder/*.cpp"),
        path.join(ANGELSCRIPT_DIR, "source/scriptmath/*.cpp"),
    }

    defines {
        "AS_NO_EXCEPTIONS",
    }

    configuration { "x64", "vs*" }
        prebuildcommands {
            "ml64.exe /c  /nologo /Fo as_callfunc_x64_msvc_asm.obj /W3 /Zi /Ta " ..
                path.getabsolute(path.join(ANGELSCRIPT_DIR, "source/as_callfunc_x64_msvc_asm.asm")),
        }
        linkoptions {
            "as_callfunc_x64_msvc_asm.obj"
        }
