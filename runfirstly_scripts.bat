@echo off

:: 取得当前路径
SET CUR_PATH="%~dp0"
SET VER="%1"

:: 获取管理员权限
:: if not "%1"=="am_admin" (powershell start -verb runas '%0' am_admin & exit /b)

:: -------------------------------------- 链接配置 -------------------------------------------
if not exist %CUR_PATH%output\%VER%\Cfgs (
    mklink /d %CUR_PATH%output\%VER%\Cfgs %CUR_PATH%Service\Cfgs
	
)

:: ------------------------------------ 结束杂项链接 -----------------------------------------
echo Done!
ping -n 1 -w 1618 11.11.11.11 > nul
exit 0

:: -------------------------------------- 函数定义 -------------------------------------------
:: relink目录函数定义
:relink_dir
if exist %2 (
    rd /s/q %2
)
mklink /d %2 %1
goto EOF

:: relink文件函数定义
:relink_file
if exist %2 (
    del /q %2
)
mklink %2 %1
goto EOF


::remove_dir 删除目录
:remove_dir
set DelDir=%1
if exist %delDir% (
    rd /s/q %delDir%
)
goto EOF
:: ------------------------------------ 结束函数定义 -----------------------------------------

:EOF

