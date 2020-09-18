# @file gitupdate.sh
# @author EricYonng<120453674@qq.com>
# @brief gitupdate ...
#!/usr/bin/env bash

SCRIPT_PATH="$(cd $(dirname $0); pwd)"

# 远程分支名
#REMOTE_BRANCH_NAME="master"
REMOTE_BRANCH_NAME="Develop"
if [ -n "$1" ]
then
    REMOTE_BRANCH_NAME="$1"
fi

# 远程分支地址
REMOTE_NAME=$(git remote)

# 重置到FETCH_HEAD
# git reset --hard FETCH_HEAD
# 更新
git pull $REMOTE_NAME $REMOTE_BRANCH_NAME
# 恢复脚本文件权限
# sudo chmod 777 $SCRIPT_PATH/*.sh
