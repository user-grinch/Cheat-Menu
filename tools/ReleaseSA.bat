@echo off
echo --------------------------------------------------
echo Building ReleaseSA
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild CheatMenu.sln /property:Configuration=Release /t:CheatMenuSA
del %SA_DIR%"\CheatMenuSA.asi" /Q
xcopy /s "bin\CheatMenuSA.asi" %SA_DIR% /K /D /H /Y 
xcopy /s "..\resource\common\locale\English.toml" %SA_DIR%"\CheatMenuSA\locale\" /K /D /H /Y 