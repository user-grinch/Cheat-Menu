@echo off
echo --------------------------------------------------
echo Building DebugAll
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild CheatMenu.sln /property:Configuration=Debug
del %SA_DIR%"\CheatMenuSA.asi" /Q
del %SA_DIR%"\CheatMenuSA.pdb" /Q
del %SA_DIR%"\CheatMenuVC.asi" /Q
del %SA_DIR%"\CheatMenuVC.pdb" /Q
del %SA_DIR%"\CheatMenuIII.asi" /Q
del %SA_DIR%"\CheatMenuIII.pdb" /Q
%systemroot%\System32\xcopy /s "build\bin\CheatMenuSA.asi" %SA_DIR% /K /D /H /Y 
%systemroot%\System32\xcopy /s "build\bin\CheatMenuVC.asi" %VC_DIR% /K /D /H /Y 
%systemroot%\System32\xcopy /s "build\bin\CheatMenuIII.asi" %III_DIR% /K /D /H /Y 
%systemroot%\System32\xcopy /s "build\bin\CheatMenuSA.pdb" %SA_DIR% /K /D /H /Y 
%systemroot%\System32\xcopy /s "build\bin\CheatMenuVC.pdb" %VC_DIR% /K /D /H /Y 
%systemroot%\System32\xcopy /s "build\bin\CheatMenuIII.pdb" %III_DIR% /K /D /H /Y 