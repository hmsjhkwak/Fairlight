--
-- Copyright 2010-2018 Branimir Karadzic. All rights reserved.
-- License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
--

project "texturec"
	uuid (os.uuid("texturec"))
	kind "ConsoleApp"

	includedirs {
		path.join(BX_DIR,   "include"),
		path.join(BIMG_DIR, "include"),
		path.join(BGFX_DIR, "include"),
		path.join(BIMG_DIR, "3rdparty/iqa/include"),
		path.join(BIMG_DIR, "tools/texturec/include"),
	}

	files {
		path.join(BIMG_DIR, "tools/texturec/include/texturec.h"),
		path.join(BIMG_DIR, "tools/texturec/src/texturec.cpp"),
	}

	links {
		"bimg_decode",
		"bimg_encode",
		"bimg",
		"bx",
	}

	configuration { "mingw-*" }
		targetextension ".exe"
		links {
			"psapi",
		}

	configuration { "osx" }
		links {
			"Cocoa.framework",
		}

	configuration { "vs20*" }
		links {
			"psapi",
		}

	configuration {}


project "texturec-dll"
	uuid (os.uuid("texturec-dll"))
	kind "SharedLib"
	targetname "texc"

	includedirs {
		path.join(BX_DIR,   "include"),
		path.join(BIMG_DIR, "include"),
		path.join(BGFX_DIR, "include"),
		path.join(BIMG_DIR, "3rdparty/iqa/include"),
		path.join(BIMG_DIR, "tools/texturec/include"),
	}

	files {
		path.join(BIMG_DIR, "tools/texturec/include/texturec.h"),
		path.join(BIMG_DIR, "tools/texturec/src/texturec.cpp"),
	}

	links {
		"bimg_decode",
		"bimg_encode",
		"bimg",
		"bx",
	}

	configuration { "mingw-*" }
		targetextension ".exe"
		links {
			"psapi",
		}

	configuration { "osx" }
		links {
			"Cocoa.framework",
		}

	configuration { "vs20*" }
		links {
			"psapi",
		}

	configuration {}