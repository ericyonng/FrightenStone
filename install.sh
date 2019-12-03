# @file install.sh
# @author EricYonng<120453674@qq.com>
# @brief install so...
#!/usr/bin/env bash

# 变量与等号之间不可以有空格，否则会被当成命令
SCRIPT_PATH=$(cd '$(dirname "$0")'; pwd)
DEBUG_LIBS=("libFrightenstone_debug.so")
RELEASE_LIBS=("libFrightenstone.so")

if [ -n "$1" ]
then
VER="$1"
OUTPUT_DIR=$SCRIPT_PATH/output/gmake/$VER

	if [ $VER = "debug" ]
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
	
	# 开启coredump
	echo "set tmp unlimited"
    ulimit -c unlimited
	SET_PATTEN="${OUTPUT_DIR}/core_%e_%t_p_s"
	CORE_PATTERN_PATH="/proc/sys/kernel/core_pattern"
	echo "set tmp coredump format"
	if grep -qE ".*${SET_PATTEN}.*" ${CORE_PATTERN_PATH}
	then
	    echo "core dump name format is already set"
	else
	    echo "will set core dump name format"
		echo "${SET_PATTEN}" > ${CORE_PATTERN_PATH}
	fi
	
	# coredump设置永久生效
	LIMITS_CONF_PATH="/etc/security/limits.conf"
	echo "set forever unlimited"
	if grep -qE ".*@root soft core unlimited.*" ${LIMITS_CONF_PATH}
	then
	    echo "it is alread setted unlimited forever"
	else
	    echo "will set unlimited forever"
		echo -e "@root soft core unlimited \n@root hard core unlimited\n" > ${LIMITS_CONF_PATH}
	fi
	
    SYSCTL_CONF_PATH="/etc/sysctl.conf"
	echo "set forever coredump format"
	if grep -qE ".*${SET_PATTEN}.*" ${SYSCTL_CONF_PATH}
	then
	    echo "core dump format is alread setted forever"
	else
	    echo "will set core dump format forever"
		echo -e "kernel.core_pattern=${SET_PATTEN}\nkernel.core_uses_pid=0\n" > ${SYSCTL_CONF_PATH}
		sysctl -p
	fi
fi





