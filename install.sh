# @file install.sh
# @author EricYonng<120453674@qq.com>
# @brief install so...
#!/usr/bin/env bash

# 变量与等号之间不可以有空格，否则会被当成命令
SCRIPT_PATH=$(cd '$(dirname "$0")'; pwd)
DEBUG_LIBS={"libFrightenstone_debug.so"}
RELEASE_LIBS={"libFrightenstone.so"}
OPEN_COREDUMP="opencoredump"
COREDUMPFLAG="$2"

if [ -n "$1" ]
then
VER="$1"
OUTPUT_DIR=${SCRIPT_PATH}/output/gmake/
ls ${OUTPUT_DIR}

	if [ $VER = "debug" ]
	then
		# 创建debug版本的so连接符号
		for libName in $DEBUG_LIBS
		do
			rm -f /usr/lib64/$libName
			ln -sv ${OUTPUT_DIR}/$libName /usr/lib64/$libName
		done
	else
		# 创建release版本的so连接符号
		for libName in $RELEASE_LIBS
		do
			rm -f /usr/lib64/$libName
			ln -sv ${OUTPUT_DIR}/$libName /usr/lib64/$libName
		done
	fi
	
	# 开启coredump
#	echo "set tmp unlimited"
#    ulimit -c unlimited
#	SET_PATTEN="${OUTPUT_DIR}/core_%e_time%t_pid%p_sinal%s"
#	CORE_PATTERN_PATH="/proc/sys/kernel/core_pattern"
#	echo "set tmp coredump format"
#	if grep -qE ".*${SET_PATTEN}.*" ${CORE_PATTERN_PATH}
#	then
#	    echo "core dump name format is already set"
#	else
#	    echo "will set core dump name format"
#		echo "${SET_PATTEN}" > ${CORE_PATTERN_PATH}
#	fi
	
	# 参数设置
	LIMITS_CONF_PATH="/etc/security/limits.conf"
	SET_PATTEN="${OUTPUT_DIR}/core_%e_timestamp_%t_pid_%p_signal_%s"   
	SYSCTL_CONF_PATH="/etc/sysctl.conf"
	
	# coredump设置永久生效
	if [ -n "$2" ]
	then
		if [ $COREDUMPFLAG = $OPEN_COREDUMP ]
		then
			# 设置无限制
			echo "set forever unlimited"
			if grep -qE ".*@root soft core unlimited.*" ${LIMITS_CONF_PATH}
			then
				echo "it is already setted unlimited forever"
			else
				echo "will set unlimited forever"
				echo -e "@root soft core unlimited \n@root hard core unlimited\n" >> ${LIMITS_CONF_PATH}
			fi
			
			# 设置cordump 系统级别	 
			echo "set forever coredump format"
			if grep -qE ".*${SET_PATTEN}.*" ${SYSCTL_CONF_PATH}
			then
				echo "core dump format is already setted forever"
			else
				echo "will set core dump format forever"
				echo -e "kernel.core_pattern=${SET_PATTEN}\nkernel.core_uses_pid=0\n" >> ${SYSCTL_CONF_PATH}
				# sysctl -p
			fi
		fi
	fi	
	
	# 修改当前系统最大文件描述符打开数量
	echo "modify file describe quantity"
	sed -i "/hard nofile/d" ${LIMITS_CONF_PATH}
	sed -i "/soft nofile/d" ${LIMITS_CONF_PATH}
	echo -e "* hard nofile 1024000 \n* soft nofile 1024000 \n" >> ${LIMITS_CONF_PATH}
	
	# 修改系统级的最大文件描述符数量
	echo "modify file describe quantity at system level"	
	sed -i "/fs.file-max/d" ${SYSCTL_CONF_PATH}
	echo -e "fs.file-max=1024000 \n" >> ${SYSCTL_CONF_PATH}	
	
	# 生效
	sysctl -p
fi

ln -sv $SCRIPT_PATH/Service/Cfgs ${OUTPUT_DIR}/Cfgs






