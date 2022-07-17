@echo off
echo --------------------------------------------------
echo Building ReleaseVC
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild CheatMenu.sln /property:Configuration=Release /t:CheatMenuVC
del %VC_DIR%"\CheatMenuVC.asi" /Q
xcopy /s "bin\CheatMenuVC.asi" %VC_DIR% /K /D /H /Y 
xcopy /s "..\resource\common\locale\English.toml" %VC_DIR%"\CheatMenuVC\locale\" /K /D /H /Y 