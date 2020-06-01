@echo off

:: 取得当前路径
SET CUR_PATH="%~dp0"

:: 获取管理员权限
:: if not "%1"=="am_admin" (powershell start -verb runas '%0' am_admin & exit /b)

:: 删除之前的link版本
if exist %CUR_PATH%output\vs2017\Cfgs\ (
    rd /s/q %CUR_PATH%output\vs2017\Cfgs\
)

:: -------------------------------------- 链接配置 -------------------------------------------
:: 链接自动生成配置代码及数据
REM call :relink_dir %CUR_PATH%config\csharp\client\pc %CUR_PATH%client\trunk\ClientAssembly\GameBusiness\src\Com\Config\Cfg
call :relink_dir %CUR_PATH%Service\Cfgs %CUR_PATH%output\vs2017\Cfgs

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

