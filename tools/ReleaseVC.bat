@echo off
echo --------------------------------------------------
echo Building ReleaseVC
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild CheatMenu.sln /property:Configuration=Release /t:CheatMenuVC
%systemroot%\System32\xcopy /s "bin\CheatMenuVC.asi" %VC_DIR% /K /D /H /Y 
