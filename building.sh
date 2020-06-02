# @file building.sh
# @author EricYonng<120453674@qq.com>
# @brief 3rd scripts
#!/usr/bin/env bash

# 变量与等号之间不可以有空格，否则会被当成命令
SCRIPT_PATH="$(cd $(dirname $0); pwd)"
DEBUG_LIBS=("libFrightenstone_debug.so")
RELEASE_LIBS=("libFrightenstone.so")
OPEN_COREDUMP="opencoredump"
COREDUMPFLAG="$2"

if [ -n "$1" ]
then
VER="$1"
OUTPUT_DIR=${SCRIPT_PATH}/output/gmake/

	if [ $VER = "debug" ]
	then
		# 创建debug版本的so连接符号
		for libName in $DEBUG_LIBS
		do
			sudo rm -f /usr/lib64/$libName
			sudo ln -sv ${OUTPUT_DIR}/$libName /usr/lib64/$libName
		done
	else
		# 创建release版本的so连接符号
		for libName in $RELEASE_LIBS
		do
			sudo rm -f /usr/lib64/$libName
			sudo ln -sv ${OUTPUT_DIR}/$libName /usr/lib64/$libName
		done
	fi
fi

sudo ln -sv $SCRIPT_PATH/Service/Cfgs ${OUTPUT_DIR}/Cfgs