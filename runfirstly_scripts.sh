# @file linuxmakefile_build.sh
# @author EricYonng<120453674@qq.com>
# @brief 3rd scripts
#!/usr/bin/env bash

# 版本
# VER="debug"
# if [ -n "$1" ]
# then
# VER="$1"
#    if [ $VER != "debug" ]
#    then
#	    $VER="release"
#	if
# if

# 路径
SCRIPT_PATH="$(cd $(dirname $0); pwd)"

# install szrz
# yum -y install lrzsz

# install uuid soft package
# install openssl

sudo rm -rf /usr/local/openssl
sudo mkdir /usr/local/openssl
sudo rm -rf /usr/include/openssl
sudo rm -rf /usr/lib/libcrypto.so
sudo rm -rf /usr/lib/libssl.so
sudo rm -rf /usr/lib/libcrypto.so.1.0.0
sudo rm -rf /usr/lib/libssl.so.1.0.0

sudo cp -fR $SCRIPT_PATH/3rd/openssl/linux/* /usr/local/openssl/
sudo ln -sv /usr/local/openssl/include/openssl /usr/include/openssl
sudo ln -sv /usr/local/openssl/lib/libcrypto.so /usr/lib/libcrypto.so
sudo ln -sv /usr/local/openssl/lib/libssl.so /usr/lib/libssl.so
sudo ln -sv /usr/local/openssl/lib/libcrypto.so.1.0.0 /usr/lib/libcrypto.so.1.0.0
sudo ln -sv /usr/local/openssl/lib/libssl.so.1.0.0 /usr/lib/libssl.so.1.0.0


# coredump设置永久生效
OUTPUT_DIR=$SCRIPT_PATH/output/gmake/
LIMITS_CONF_PATH="/etc/security/limits.conf"
echo "set forever unlimited"
if grep -qE ".*@root soft core unlimited.*" ${LIMITS_CONF_PATH}
then
	echo "it is already setted unlimited forever"
else
	echo "will set unlimited forever"
	echo -e "@root soft core unlimited \n@root hard core unlimited\n" >> ${LIMITS_CONF_PATH}
fi

# 设置cordump 系统级别
SET_PATTEN="${OUTPUT_DIR}/core_%e_timestamp_%t_pid_%p_signal_%s"    
echo "set forever coredump format"
SYSCTL_CONF_PATH="/etc/sysctl.conf"
if grep -qE ".*${SET_PATTEN}.*" ${SYSCTL_CONF_PATH}
then
	echo "core dump format is already setted forever"
else
	echo "will set core dump format forever"
	echo -e "kernel.core_pattern=${SET_PATTEN}\nkernel.core_uses_pid=0\n" >> ${SYSCTL_CONF_PATH}
	# sysctl -p
fi
#	sysctl -p

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
