--  @file   fixcpp17_forwin.lua
-- #########################################################################
-- Global settings

-- platform judge
IS_WINDOWS = string.match(_ACTION, 'vs') ~= nil

-- All libraries output directory
PROJ_MODULE_NAME = {"Frightenstone", "TestSuit", "FS_MyClient"}
FS_BUILD_DIR = "../../build/"

-----------------------------------------------------------------------------------------------------------
	
if IS_WINDOWS then

    for i = 1, #PROJ_MODULE_NAME do
	    -- 工程文件路径
	    local solutionPath = FS_BUILD_DIR .. _ACTION .. "/" .. PROJ_MODULE_NAME[i] .. ".vcxproj";
	
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

end

