# @file config_env.sh
# @author EricYonng<120453674@qq.com>
# @brief 3rd scripts
#!/usr/bin/env bash

# 路径
SCRIPT_PATH="$(cd $(dirname $0); pwd)"
# 配置环境变量
# sudo export PATH=$PATH:$SCRIPT_PATH/3rd/openssl/linux/include:$SCRIPT_PATH/3rd/openssl/linux/lib

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
