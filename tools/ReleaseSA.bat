@echo off
echo --------------------------------------------------
echo Building ReleaseSA
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild CheatMenu.sln /property:Configuration=Release /t:CheatMenuSA
%systemroot%\System32\xcopy /s "bin\CheatMenuSA.asi" %SA_DIR% /K /D /H /Y 
