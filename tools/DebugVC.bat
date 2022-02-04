@echo off
echo --------------------------------------------------
echo Building DebugVC
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild CheatMenu.sln /property:Configuration=Debug /t:CheatMenuVC
del %VC_DIR%"\CheatMenuVC.asi" /Q
del %VC_DIR%"\CheatMenuVC.pdb" /Q
%systemroot%\System32\xcopy /s "bin\CheatMenuVC.asi" %VC_DIR% /K /D /H /Y 
%systemroot%\System32\xcopy /s "bin\CheatMenuVC.pdb" %VC_DIR% /K /D /H /Y 
