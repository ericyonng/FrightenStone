# @file onkeybuild_inlinux.sh
# @author EricYonng<120453674@qq.com>
# @brief one key build
#!/usr/bin/env bash

BUILD_PATH="build/gmake/"
SCRIPT_PATH="$(cd $(dirname $0); pwd)"

# ver:default debug
VER="debug"
if [ -n "$1" ]
then
    if [ "$1" = "release" ]
	then
	    VER="release"
	fi
fi

# check param 1,2 if need clean
NEED_CLEAN=0
if [ -n "$1" ]
then
    if [ "$1" = "clean" ]
	then
	    NEED_CLEAN=1
	fi
fi
if [ -n "$2" ]
then
    if [ "$2" = "clean" ]
	then
	    NEED_CLEAN=1
	fi
fi

# give scripts right
sudo chmod 777 $SCRIPT_PATH/*.sh

# run firstly
source $SCRIPT_PATH/runfirstly_scripts.sh

# generate make files
source $SCRIPT_PATH/linuxmakefile_build.sh

# build
cd $SCRIPT_PATH/$BUILD_PATH
if [ NEED_CLEAN = 1 ]
then
    make clean config=$VER;
fi
make config=$VER

# quit build dir
cd $SCRIPT_PATH

# install default:opencoredump
source $SCRIPT_PATH/install.sh $VER opencoredump

