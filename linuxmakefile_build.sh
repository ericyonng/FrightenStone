# @file    LinuxPrebuild.sh
# @author  Longwei Lai<lailongwei@onemt.com.cn>
# @brief   完成linux服务器预编译, 主要生成makefile文件(从llbc项目抽取)

#!/usr/bin/env bash

# 设置相关变量 获取当前脚本路径
SCRIPT_PATH=$(cd "$(dirname "$0")"; pwd)
echo $"scriptpata="$SCRIPT_PATH
PREMAKE_SCRIPT_PATH=$SCRIPT_PATH/tools/premake
PREMAKE_TOOL=$PREMAKE_SCRIPT_PATH/linux_premake5

# 执行premake脚本 
echo "===============Generate gmake Makefiles==============="
chmod +x ${PREMAKE_TOOL}
${PREMAKE_TOOL} --file=$PREMAKE_SCRIPT_PATH/premake5.lua gmake

