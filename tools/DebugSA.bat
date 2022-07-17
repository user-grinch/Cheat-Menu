@echo off
echo --------------------------------------------------
echo Building DebugSA
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild CheatMenu.sln /property:Configuration=Debug /t:CheatMenuSA
del %SA_DIR%"\CheatMenuSA.asi" /Q
del %SA_DIR%"\CheatMenuSA.pdb" /Q
xcopy /s "bin\CheatMenuSA.asi" %SA_DIR% /K /D /H /Y 
xcopy /s "bin\CheatMenuSA.pdb" %SA_DIR% /K /D /H /Y 
xcopy /s "..\resource\common\locale\English.toml" %SA_DIR%"\CheatMenuSA\locale\" /K /D /H /Y 

