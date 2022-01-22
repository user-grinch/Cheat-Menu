@echo off
echo --------------------------------------------------
echo Building DebugSA
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild CheatMenu.sln /property:Configuration=Debug /t:CheatMenuSA
del %SA_DIR%"\CheatMenuSA.asi" /Q
del %SA_DIR%"\CheatMenuSA.pdb" /Q
%systemroot%\System32\xcopy /s "bin\CheatMenuSA.asi" %SA_DIR% /K /D /H /Y 
%systemroot%\System32\xcopy /s "bin\CheatMenuSA.pdb" %SA_DIR% /K /D /H /Y 
