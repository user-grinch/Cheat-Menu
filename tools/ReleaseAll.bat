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
xcopy /s "bin\CheatMenuSA.asi" %SA_DIR% /K /D /H /Y 
xcopy /s "bin\CheatMenuVC.asi" %VC_DIR% /K /D /H /Y 
xcopy /s "bin\CheatMenuIII.asi" %III_DIR% /K /D /H /Y 