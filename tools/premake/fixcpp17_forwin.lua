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
	
if IS_WINDOWS then

-- 开启c++17
local solutionPath = FS_BUILD_DIR .. _ACTION .. "/" .. FS_BASE_PROJ_NAME .. ".vcxproj";
local solutionFile = io.open(solutionPath, "r")
local str = solutionFile:read("a")
local finalProj = string.gsub(str, "stdcpp14", "stdcpp17")
solutionFile:close()
solutionFile = io.open(solutionPath, "w")
solutionFile:write(finalProj)
solutionFile:close()
end

