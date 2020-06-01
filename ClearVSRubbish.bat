echo off




rem 递归删除当前文件夹（".\"或者"./"）下指定扩展名的文件


rem 如果想要添加其他的扩展名文件，按下面del开头格式的行，继续添加对应的扩展名即可

rem 添加del扩展名需注意：如"*.res"，会把"*.resx"的文件也删掉（c#中用到"*.resx"文件）！！

rem 开头的内容为注释语句，不起作用


set /p VAR=是否删除指定的文件，请选择:[Y, N]?



rem echo %VAR%



if /i '%VAR%' == 'y' goto s1


if /i '%VAR%' == 'n' goto end



:s1


for /R .\ %%i in (.\) do (cd %%i
	


	del /f /s /q *.sdf
	del /f /s /q *.pch

rem 下面是准备删除【debug】和【release】文件夹的内容，但谨慎删除！！


rem rd /s/q Release
rem rd /s/q Release_Unicode
rem rd /s/q Debug
rd /s/q ipch


)



:end


pause
