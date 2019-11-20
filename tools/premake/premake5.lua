--  @file   premake5.lua
-- #########################################################################
-- Global compile settings

-- python tool define
IS_WINDOWS = string.match(_ACTION, 'vs') ~= nil
local PY = IS_WINDOWS and "$(ProjectDir)../../tools/py.exe" or "python"

-- All libraries output directory
FS_OUTPUT_BASE_DIR = "../../output/" .. _ACTION
if IS_WINDOWS then
    FS_OUTPUT_DIR = FS_OUTPUT_BASE_DIR .. "/$(Configuration)"
else
    FS_OUTPUT_DIR = FS_OUTPUT_BASE_DIR .. "/$(config)"
end

-- Common functional functions define
-- Enable multithread compile
function enable_multithread_comp()
    filter { "system:windows" }
        flags { "MultiProcessorCompile", "NoMinimalRebuild" }
    filter {}
end

-- Set optimize options.
function set_optimize_opts()
    filter { "configurations:debug*", "language:c++", "system:not windows" }
        buildoptions {
            "-ggdb -g",
        }
    filter {}

    filter { "configurations:debug*", "language:c++", "system:windows" }
        runtime "Debug"
        optimize "Debug"
    filter {}

    filter { "configurations:debug*", "language:not c++" }
        optimize "Debug"
    filter {}

    filter { "configurations:release*" }
        optimize "Speed"
    filter {}
end

-- zlib library:
-- local ZLIB_LIB = "../../FS/3rd_party/zlib"
-- #########################################################################

workspace ("FS_" .. _ACTION)
    -- location define
    location ("../../build/" .. _ACTION)
    -- target directory define
    targetdir (FS_OUTPUT_DIR)

    -- configurations
    configurations { "release32", "debug32", "release64", "debug64" }

    -- architecture
    filter { "configurations:*32" }
        architecture "x86"
    filter {}
    filter { "configurations:*64" }
        architecture "x86_64"
    filter {}

    -- defines
    filter { "configurations:debug*" }
        defines {
            "DEBUG"
        }
    filter {}

    -- control symbols
    filter { "system:macosx", "language:c++" }
        symbols("On")
    filter {}

    -- characterset architecture
    filter { "language:c++" }
        characterset "MBCS"
    filter {}

    -- disable some warnings
    filter { "system:windows", "language:c++" }
        disablewarnings { "4091" }
    filter {}

-- ****************************************************************************
-- FS core library compile setting
project "fsbase"
    -- language, kind
    language "c++"
    kind "SharedLib"

    -- symbols
    symbols "On"

    -- files
    files {
        "../../base/**.h",
		"../../base/**.c",
		"../../base/**.cpp",
        "../../base/base/**.h",
		"../../base/base/**.c",
		"../../base/base/**.cpp",
		"../../base/common/**.h",
		"../../base/common/**.c",
		"../../base/common/**.cpp",
		"../../3rd/**.h",
		"../../3rd/**.c",
		"../../3rd/**.cpp",
    }

	-- macos需要额外添加
    filter { "system:macosx" }
    files {
        "../../base/**.mm",
    }
    filter {}

    -- includedirs
    includedirs {
        "../../3rd/openSSL/staticlib/$(Configuration)/include",
		"../../",
		"../../base/base",
     }

    -- target prefix 前缀
    targetprefix "fs"

    -- links
    filter { "system:linux" }
        links {
            "rt",
            "uuid",
        }

    filter { "system:windows" }
        links {
            "ws2_32",
            "Mswsock",
            "DbgHelp",
        }

    filter { "system:macosx" }
        links {
            "iconv",
        }
    filter {}

    -- flags
    filter { "system:not windows" }
        buildoptions {
            "-fvisibility=hidden",
        }
    filter {}

    -- optimize
    set_optimize_opts()

    -- debug target suffix define
    filter { "configurations:debug*" }
        targetsuffix "_debug"
    filter {}

    -- enable multithread compile
    enable_multithread_comp()

-- ****************************************************************************
-- core library testsuite compile setting
project "TestSuit"
    -- language, kind
    language "c++"
    kind "ConsoleApp"

    -- symbols
    symbols "On"

    -- dependents
    dependson {
        "fsbase",
    }

    -- files
    files {
        "../../TestSuit/**.h",
        "../../TestSuit/**.cpp",
    }

    -- includedirswrap\csFS\csharp\script_tools
    includedirs {
	    "../../",
		"../../TestSuit/TestSuit/",
    }

    -- links
    libdirs { FS_OUTPUT_DIR }
    filter { "system:linux" }
        links {
            "dl",
        }
    filter {}

    filter { "system:not windows", "configurations:debug*" }
        links {
            "fsbase_debug",
        }
    filter {}

    filter { "system:not windows", "configurations:release*" }
        links {
            "fsbase",
        }
    filter {}

    filter { "system:windows" }
        links {
            "ws2_32",
        }
    filter {}

    filter { "system:windows", "configurations:debug*" }
        links {
            "fsbase_debug",
        }
    filter {}

    filter { "system:windows", "configurations:release*" }
        links {
            "fsbase",
        }
    filter {}

    -- debug target suffix define
    filter { "configurations:debug*" }
        targetsuffix "_debug"
    filter {}

    -- enable multithread compile
    enable_multithread_comp()

    -- warnings
    filter { "system:not windows" }
        disablewarnings {
            "invalid-source-encoding",
        }
    filter {}

    -- optimize
    set_optimize_opts()

