# @file install.sh
# @author EricYonng<120453674@qq.com>
# @brief run svr...
#!/usr/bin/env bash

SCRIPT_PATH="$(cd $(dirname $0); pwd)"
DEBUG_BINS=("TestSuit_debug")
RELEASE_BINS=("TestSuit")
RUN_PATH=${SCRIPT_PATH}/output/gmake

if [ -n "$1" ]
then
VER="$1"
	if [ $VER = "debug" ]
	then
		# debug版本的可执行文件
		for binName in $DEBUG_BINS
		do
		    nohup ${RUN_PATH}/$binName >myout.file 2>&1 &
			echo "$binName run sucess"
		done
	else
		# release版本的可执行文件
		for binName in $RELEASE_BINS
		do
            nohup ${RUN_PATH}/$binName >myout.file 2>&1 &
			echo "$binName run sucess"
		done
	fi
else
		# release版本的可执行文件
	for binName in $DEBUG_BINS
	do
        nohup ${RUN_PATH}/$binName >myout.file 2>&1 &
		echo "$binName run sucess"
	done
fi

