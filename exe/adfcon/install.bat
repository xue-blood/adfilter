echo off
copy /Y adfilter.sys C:\Windows\System32\Drivers\adfilter.sys

sc create adfilter binpath= system32\drivers\adfilter.sys start= auto type= kernel

reg add HKLM\SYSTEM\CurrentControlSet\services\adfilter /v Pause /t REG_DWORD /d 0 /f
reg add HKLM\SYSTEM\CurrentControlSet\services\adfilter /v SysFilePath /t REG_SZ /d %~dp0sys.txt /f
reg add HKLM\SYSTEM\CurrentControlSet\services\adfilter /v UserFilePath /t REG_SZ /d %~dp0user.txt /f
reg add HKLM\SYSTEM\CurrentControlSet\services\adfilter /v ExceptFilePath /t REG_SZ /d %~dp0except.txt /f


net start adfilter