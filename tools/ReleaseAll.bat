@echo off
echo --------------------------------------------------
echo Building ReleaseAll
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild CheatMenu.sln /property:Configuration=Release
del %SA_DIR%"\CheatMenuSA.asi" /Q
del %SA_DIR%"\CheatMenuVC.asi" /Q
del %SA_DIR%"\CheatMenuIII.asi" /Q
%systemroot%\System32\xcopy /s "build\bin\CheatMenuSA.asi" %SA_DIR% /K /D /H /Y 
%systemroot%\System32\xcopy /s "build\bin\CheatMenuVC.asi" %VC_DIR% /K /D /H /Y 
%systemroot%\System32\xcopy /s "build\bin\CheatMenuIII.asi" %III_DIR% /K /D /H /Y 