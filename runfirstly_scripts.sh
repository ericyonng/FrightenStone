# @file linuxmakefile_build.sh
# @author EricYonng<120453674@qq.com>
# @brief 3rd scripts
#!/usr/bin/env bash

SCRIPT_PATH=$(cd '$(dirname "$0")'; pwd)
mkdir /usr/local/openssl
rm -rf /usr/local/openssl/*
rm -rf /usr/include/openssl
rm -rf /usr/lib64/openssl/libcrypto.so
rm -rf /usr/lib64/openssl/libssl.so

cp -fR $SCRIPT_PATH/3rd/openssl/linux/* /usr/local/openssl/
ln -sv /usr/local/openssl/include/openssl /usr/include/openssl
ln -sv /usr/local/openssl/lib/libcrypto.so.1.0.0 /usr/lib64/libcrypto.so
ln -sv /usr/local/openssl/lib/libssl.so.1.0.0 /usr/lib64/libssl.so