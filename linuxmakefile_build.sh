# @file linuxmakefile_build.sh
# @author EricYonng<120453674@qq.com>
# @brief generate makefile
#!/usr/bin/env bash

SCRIPT_PATH=$(cd '$(dirname "$0")'; pwd)
PREMAKE_SCRIPT_PATH=$SCRIPT_PATH/tools/premake
PREMAKE_TOOL=$PREMAKE_SCRIPT_PATH/linux_premake5

echo "=======================Generate gmake Makefiles==================="
chmod +x ${PREMAKE_TOOL}
${PREMAKE_TOOL} --file=$PREMAKE_SCRIPT_PATH/premake5.lua gmake
