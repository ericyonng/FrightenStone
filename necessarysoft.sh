# @file necessarysoft.sh
# @author EricYonng<120453674@qq.com>
# @brief install some necessary softs
#!/usr/bin/env bash

yum -y install epel-release				# 丰富的软件源
yum -y install gcc
yum -y install gcc-c++
yum -y install gdb						# 调试
yum -y install lrzsz					# 从linux环境中传输文件
yum -y install libuuid-devel.x86_64		# uuid 唯一id生成
yum -y install iftop					# iftop 流量监控
yum -y update							# 更新软件包

