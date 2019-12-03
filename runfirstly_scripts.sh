# @file linuxmakefile_build.sh
# @author EricYonng<120453674@qq.com>
# @brief 3rd scripts
#!/usr/bin/env bash

SCRIPT_PATH=$(cd '$(dirname "$0")'; pwd)
mkdir /usr/local/openssl
rm -rf /usr/local/openssl/*
rm -rf /usr/include/openssl
rm -rf /usr/lib64/libcrypto.so
rm -rf /usr/lib64/libssl.so
rm -rf /usr/lib64/libcrypto.so.1.0.0
rm -rf /usr/lib64/libssl.so.1.0.0

cp -fR $SCRIPT_PATH/3rd/openssl/linux/* /usr/local/openssl/
ln -sv /usr/local/openssl/include/openssl /usr/include/openssl
ln -sv /usr/local/openssl/lib/libcrypto.so.1.0.0 /usr/lib64/libcrypto.so
ln -sv /usr/local/openssl/lib/libssl.so.1.0.0 /usr/lib64/libssl.so
ln -sv /usr/local/openssl/lib/libcrypto.so.1.0.0 /usr/lib64/libcrypto.so.1.0.0
ln -sv /usr/local/openssl/lib/libssl.so.1.0.0 /usr/lib64/libssl.so.1.0.0

# 开启coredump
keystr="ulimit -S"
patten='.*'.$keystr'*'
destpath=$(find /etc/ -regex '.*' -type f  -exec grep -qE "${patten}" {} \; -print)
echo $destpath
ulimit -S -c unlimited
echo -e "\n# enable coredump whitch unlimited file-size for allusers\n* ulimit -S -c unlimited > /dev/null 2>&1" >> $destpath

