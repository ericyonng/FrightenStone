# @file gitcommitfilemode.sh
# @author EricYonng<120453674@qq.com>
# @brief 提交shell脚本权限
#!/usr/bin/env bash

# 路径
SCRIPT_PATH="$(cd $(dirname $0); pwd)"

# 远程分支名
REMOTE_BRANCH_NAME="master"
if [ -n "$1" ]
then
    REMOTE_BRANCH_NAME="$1"
fi

# 远程分支地址
REMOTE_NAME=$(git remote)

sudo chmod 777 $SCRIPT_PATH/*.sh
git update-index --chmod +x *.sh
git commit -m "commit shell file mode"
git push $REMOTE_NAME $REMOTE_BRANCH_NAME