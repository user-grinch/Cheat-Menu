@echo off
echo --------------------------------------------------
echo Building ReleaseAll
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild CheatMenu.sln /property:Configuration=Release
del %SA_DIR%"\CheatMenuSA.asi" /Q
del %VC_DIR%"\CheatMenuVC.asi" /Q
del %III_DIR%"\CheatMenuIII.asi" /Q
%systemroot%\System32\xcopy /s "bin\CheatMenuSA.asi" %SA_DIR% /K /D /H /Y 
%systemroot%\System32\xcopy /s "bin\CheatMenuVC.asi" %VC_DIR% /K /D /H /Y 
%systemroot%\System32\xcopy /s "bin\CheatMenuIII.asi" %III_DIR% /K /D /H /Y 