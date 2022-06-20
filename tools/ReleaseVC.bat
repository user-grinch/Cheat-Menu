@echo off
echo --------------------------------------------------
echo Building ReleaseVC
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild CheatMenu.sln /property:Configuration=Release /t:CheatMenuVC
del %VC_DIR%"\CheatMenuVC.asi" /Q
xcopy /s "bin\CheatMenuVC.asi" %VC_DIR% /K /D /H /Y 
