@echo off

:: ȡ�õ�ǰ·��
SET CUR_PATH="%~dp0"
SET VER="%1"

:: ��ȡ����ԱȨ��
:: if not "%1"=="am_admin" (powershell start -verb runas '%0' am_admin & exit /b)

:: -------------------------------------- �������� -------------------------------------------
if not exist %CUR_PATH%output\%VER%\Cfgs (
    mklink /d %CUR_PATH%output\%VER%\Cfgs %CUR_PATH%Service\Cfgs
	
)

:: ------------------------------------ ������������ -----------------------------------------
echo Done!
ping -n 1 -w 1618 11.11.11.11 > nul
exit 0

:: -------------------------------------- �������� -------------------------------------------
:: relinkĿ¼��������
:relink_dir
if exist %2 (
    rd /s/q %2
)
mklink /d %2 %1
goto EOF

:: relink�ļ���������
:relink_file
if exist %2 (
    del /q %2
)
mklink %2 %1
goto EOF


::remove_dir ɾ��Ŀ¼
:remove_dir
set DelDir=%1
if exist %delDir% (
    rd /s/q %delDir%
)
goto EOF
:: ------------------------------------ ������������ -----------------------------------------

:EOF

