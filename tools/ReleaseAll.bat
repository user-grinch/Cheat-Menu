@echo off
echo --------------------------------------------------
echo Building ReleaseAll
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild CheatMenu.sln /property:Configuration=Release
%systemroot%\System32\xcopy /s "build\bin\CheatMenuSA.asi" %SA_DIR% /K /D /H /Y 
%systemroot%\System32\xcopy /s "build\bin\CheatMenuVC.asi" %VC_DIR% /K /D /H /Y 
%systemroot%\System32\xcopy /s "build\bin\CheatMenuIII.asi" %III_DIR% /K /D /H /Y 