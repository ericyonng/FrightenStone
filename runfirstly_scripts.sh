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
sudo rm -rf /usr/lib64/libcrypto.so
sudo rm -rf /usr/lib64/libssl.so
sudo rm -rf /usr/lib64/libcrypto.so.1.0.0
sudo rm -rf /usr/lib64/libssl.so.1.0.0

sudo cp -fR $SCRIPT_PATH/3rd/openssl/linux/* /usr/local/openssl/
sudo ln -sv /usr/local/openssl/include/openssl /usr/include/openssl
sudo ln -sv /usr/local/openssl/lib/libcrypto.so /usr/lib64/libcrypto.so
sudo ln -sv /usr/local/openssl/lib/libssl.so /usr/lib64/libssl.so
sudo ln -sv /usr/local/openssl/lib/libcrypto.so.1.0.0 /usr/lib64/libcrypto.so.1.0.0
sudo ln -sv /usr/local/openssl/lib/libssl.so.1.0.0 /usr/lib64/libssl.so.1.0.0

