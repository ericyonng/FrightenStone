--  @file   premake5.lua
-- #########################################################################
-- Global compile settings

-- python tool define
IS_WINDOWS = string.match(_ACTION, 'vs') ~= nil
-- header directory
FS_HEADER_DIR = "../../frame/frameheader/"
-- All libraries output directory
FS_OUTPUT_BASE_DIR = "../../output/" .. _ACTION
-- root directory
FS_ROOT_DIR = "../../"
-- build directory
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
            "-std=c++17 -ggdb -g",
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


-- lib include实现
function include_libfs(do_post_build)	
    -- includedirs
    includedirs {
        FS_ROOT_DIR .. "/frame/include/",
    }

    -- files
    files {
        FS_ROOT_DIR .. "/frame/include/**.h",
    }

    -- libdirs(linux)
    filter { "system:linux"}
		includedirs {
        "/usr/include/",
		}
        libdirs {
            FS_ROOT_DIR .. "/usr/lib64/",
        }
    filter {}

    -- links(not windows)
    filter { "system:not windows", "configurations:debug*" }
        links {
            "Frightenstone_debug",
        }
    filter {}
    filter { "system:not windows", "configurations:release*" }
        links {
            "Frightenstone",
        }
    filter {}

    -- links(windows)
    filter { "system:windows" }
        links {
            -- "ws2_32",
        }
    filter {}
    filter { "system:windows", "configurations:debug*" }
        links {
            "libFrightenstone_debug",
        }
    filter {}
    filter { "system:windows", "configurations:release*" }
        links {
            "libFrightenstone",
        }
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
    configurations {"debug", "release"}

    -- architecture 全部配置都生成64位程序
    filter { "configurations:*" }
        architecture "x86_64"
    filter {}

    -- defines
    filter { "configurations:debug*" }
        defines {
            "DEBUG",
			"_DEBUG",
        }
    filter {}
	
    filter { "configurations:release*" }
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
project "Frightenstone"
    -- language, kind
    language "c++"
    kind "SharedLib"
	pchheader("stdafx.h")
	pchsource(FS_HEADER_DIR .. "stdafx.cpp")
	
    -- symbols
    symbols "On"

    -- files
    files {
        "../../frame/**.h",
		"../../frame/**.c",
		"../../frame/**.cpp",
		"../../3rd/*.h",
		"../../3rd/tiny-utf8/lib/*.cpp",
    }

	defines { "__FRIGHTEN_STONE_FRAME_DLL__" }
	filter{ "system:windows"}		
		libdirs { 
			FS_ROOT_DIR .. "3rd/openssL/",
			FS_ROOT_DIR .. "3rd/"
		}
		includedirs{
		"../../3rd/openSSL/staticlib/$(Configuration)/include",
		}
	filter{}
	
	-- macos需要额外添加
    filter { "system:macosx" }
    files {
        "../../frame/**.mm",
    }
    filter {}

    -- includedirs
    includedirs {
		"../../",
		"../../frame/include/",
		"../../frame/frameheader/",
     }

    -- target prefix 前缀
    targetprefix "lib"

    -- links
    filter { "system:linux" }
	    includedirs {
        "/usr/include/",
		}
        links {
            "rt",
            "uuid",
			"pthread",
			"crypto",
			"ssl",
			"dl",
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
            "-fvisibility=hidden -std=c++17",
        }
    filter {}

    -- optimize
    set_optimize_opts()

    -- debug target suffix define
    filter { "configurations:debug*" }
        targetsuffix "_debug"
    filter {}

    -- enable multithread compile
    enable_multithread_comp("C++14")
	
	-- post build(linux)
	filter { "system:linux", "configurations:debug*"}
	postbuildmessage "Copying dependencies of frightenstnoe ..."
	postbuildcommands(string.format("%sinstall.sh debug",  FS_ROOT_DIR))
	filter {}
	
	-- post build(linux)
	filter { "system:linux", "configurations:release*"}
	postbuildmessage "Copying dependencies of frightenstnoe ..."
	postbuildcommands(string.format("%sinstall.sh release",  FS_ROOT_DIR))
	filter {}

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
        "Frightenstone",
    }

    -- files
    files {
        "../../TestSuit/**.h",
        "../../TestSuit/**.cpp",
		"../../protocols/**.h",
		"../../protocols/**.c",
		"../../protocols/**.cpp",
    }

	includedirs {
	    "../../",
		"../../frame/include/",
		"../../TestSuit/TestSuit/",
		"../../protocols/Impl/",
    }
	
	-- links
    libdirs { FS_OUTPUT_DIR }
    filter { "system:linux" }
        links {
		    "rt",
            "uuid",
			"pthread",
			"crypto",
			"ssl",
            "dl",
        }
    filter {}
	
	include_libfs(true)

    -- debug target suffix define
    filter { "configurations:debug*" }
        targetsuffix "_debug"
    filter {}

    -- enable multithread compile
    enable_multithread_comp("C++14")

    -- warnings
    filter { "system:not windows" }
        disablewarnings {
            "invalid-source-encoding",
        }
    filter {}

    -- optimize
    set_optimize_opts()
	
	-- core library testsuite compile setting
project "FS_MyClient"
    -- language, kind
    language "c++"
    kind "ConsoleApp"

	-- precompile header
	pchheader("stdafx.h")
	pchsource(FS_ROOT_DIR .. "FS_MyClient/FS_MyClient/stdafx.cpp")
	
    -- symbols
    symbols "On"

    -- dependents
    dependson {
        "Frightenstone",
    }

    -- files
    files {
        "../../FS_MyClient/**.h",
        "../../FS_MyClient/**.cpp",
		"../../protocols/**.h",
		"../../protocols/**.c",
		"../../protocols/**.cpp",
    }

	includedirs {
	    "../../",
		"../../frame/include/",
		"../../FS_MyClient/FS_MyClient/",
		"../../protocols/Impl/",
    }
	
	-- links
    libdirs { FS_OUTPUT_DIR }
    filter { "system:linux" }
        links {
		    "rt",
            "uuid",
			"pthread",
			"crypto",
			"ssl",
            "dl",
        }
    filter {}
	
	include_libfs(true)

    -- debug target suffix define
    filter { "configurations:debug*" }
        targetsuffix "_debug"
    filter {}

    -- enable multithread compile
    enable_multithread_comp("C++14")

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

