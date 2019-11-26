--  @file   premake5.lua
-- #########################################################################
-- Global compile settings

-- python tool define
IS_WINDOWS = string.match(_ACTION, 'vs') ~= nil
local PY = IS_WINDOWS and "$(ProjectDir)../../tools/py.exe" or "python"

-- header directory
FS_HEADER_DIR = "../../base/base/"

-- All libraries output directory
FS_OUTPUT_BASE_DIR = "../../output/" .. _ACTION

-- root directory
FS_ROOT_DIR = "../../"

FS_BASE_PROJ_NAME = "fsbase"

FS_BUILD_DIR = "../../build/"

if IS_WINDOWS then
    FS_OUTPUT_DIR = FS_OUTPUT_BASE_DIR .. "/$(Configuration)"
else
    FS_OUTPUT_DIR = FS_OUTPUT_BASE_DIR .. "/$(config)"
end
-----------------------------------------------------------------------------------------------------------

-- Common functional functions define
-- Enable multithread compile
function enable_multithread_comp(cppstdver)
    filter { "system:windows" }
        flags { "MultiProcessorCompile", "NoMinimalRebuild", cppstdver }
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

workspace ("Frightenstone_" .. _ACTION)
    -- location define
    location (FS_BUILD_DIR .. _ACTION)
    -- target directory define
    targetdir (FS_OUTPUT_DIR)

    -- configurations 默认64位 不输出32位
    configurations {"Debug", "Release"}

    -- architecture 全部配置都生成64位程序
    filter { "configurations:*" }
        architecture "x86_64"
    filter {}

    -- defines
    filter { "configurations:Debug*" }
        defines {
            "DEBUG"
        }
    filter {}
	
    filter { "configurations:Release*" }
        defines {
            "NDEBUG"
        }
    filter {}
	
    -- control symbols
    filter { "system:macosx", "language:c++" }
        symbols("On")
    filter {}

    -- characterset architecture 多字节字符
    filter { "language:c++" }
        characterset "MBCS"
    filter {}

    -- disable some warnings
    filter { "system:windows", "language:c++" }
        disablewarnings { "4091", "4819" }
    filter {}

-- ****************************************************************************
-- FS core library compile setting
project "fsbase"
    -- language, kind
    language "c++"
    kind "SharedLib"

    pchheader("stdafx.h")
	pchsource(FS_HEADER_DIR .. "stdafx.cpp")
	
    -- symbols
    symbols "On"

    -- files
    files {
        "../../base/**.h",
		"../../base/**.c",
		"../../base/**.cpp",
		"../../3rd/*.h",
		"../../3rd/tiny-utf8/lib/*.cpp",
    }

	filter{ "system:windows"}
		defines { "FRIGHTEN_STONE_BASE_EXPORT_BASE_DLL" }
		libdirs { 
			FS_ROOT_DIR .. "3rd/openssL/",
			FS_ROOT_DIR .. "3rd/"
		}
	filter{}
	
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
     }

    -- target prefix 前缀
    targetprefix ""

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
    filter { "configurations:Debug*" }
        targetsuffix "_debug"
    filter {}

    -- enable multithread compile
    enable_multithread_comp("C++14")

-- ****************************************************************************
-- core library testsuite compile setting
project "TestSuit"
    -- language, kind
    language "c++"
    kind "ConsoleApp"

	-- precompile header
	pchheader("stdafx.h")
	pchsource(FS_ROOT_DIR .. "TestSuit/TestSuit/stdafx.cpp")
	
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
	
if IS_WINDOWS == false then
	print("builddir = " .. FS_BUILD_DIR)
end

