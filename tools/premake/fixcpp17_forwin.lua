--  @file   fixcpp17_forwin.lua
-- #########################################################################
-- Global settings

-- platform judge
IS_WINDOWS = string.match(_ACTION, 'vs') ~= nil

-- All libraries output directory
FS_BASE_PROJ_NAME = "Frightenstone"
FS_BUILD_DIR = "../../build/"

-----------------------------------------------------------------------------------------------------------
	
if IS_WINDOWS then

	-- 工程文件路径
	local solutionPath = FS_BUILD_DIR .. _ACTION .. "/" .. FS_BASE_PROJ_NAME .. ".vcxproj";
	
	-- 读取文件内容
	local solutionFile = io.open(solutionPath, "r")
	local str = solutionFile:read("a")
	
	-- 替换成stdcpp17
	local finalProj = string.gsub(str, "stdcpp14", "stdcpp17")
	solutionFile:close()
	
	-- 重新写入文件
	solutionFile = io.open(solutionPath, "w")
	solutionFile:write(finalProj)
	solutionFile:close()
end

