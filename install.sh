# @file install.sh
# @author EricYonng<120453674@qq.com>
# @brief install so...
#!/usr/bin/env bash

# 变量与等号之间不可以有空格，否则会被当成命令
SCRIPT_PATH=$(cd '$(dirname "$0")'; pwd)
DEBUG_LIBS={"libFrightenstone_debug.so"}
RELEASE_LIBS={"libFrightenstone.so"}

if [ -n "$1" ]
then
VER="$1"
OUTPUT_DIR = $SCRIPT_PATH/output/gmake/$VER

	if [$VER = "debug"]
	then
		# 创建debug版本的so连接符号
		for libName in $DEBUG_LIBS
		do
			rm -f /usr/lib64/$libName
			ln -sv $OUTPUT_DIR/$libName /usr/lib64/$libName
		done
	else
		# 创建release版本的so连接符号
		for libName in $RELEASE_LIBS
		do
			rm -f /usr/lib64/$libName
			ln -sv $OUTPUT_DIR/$libName /usr/lib64/$libName
		done
	fi
fi





