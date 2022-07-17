@echo off
echo --------------------------------------------------
echo Building DebugAll
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild CheatMenu.sln /property:Configuration=Debug
del %SA_DIR%"\CheatMenuSA.asi" /Q
del %SA_DIR%"\CheatMenuSA.pdb" /Q
del %VC_DIR%"\CheatMenuVC.asi" /Q
del %VC_DIR%"\CheatMenuVC.pdb" /Q
del %III_DIR%"\CheatMenuIII.asi" /Q
del %III_DIR%"\CheatMenuIII.pdb" /Q
xcopy /s "bin\CheatMenuSA.asi" %SA_DIR% /K /D /H /Y 
xcopy /s "bin\CheatMenuVC.asi" %VC_DIR% /K /D /H /Y 
xcopy /s "bin\CheatMenuIII.asi" %III_DIR% /K /D /H /Y 
xcopy /s "bin\CheatMenuSA.pdb" %SA_DIR% /K /D /H /Y 
xcopy /s "bin\CheatMenuVC.pdb" %VC_DIR% /K /D /H /Y 
xcopy /s "bin\CheatMenuIII.pdb" %III_DIR% /K /D /H /Y 
xcopy /s "..\resource\common\locale\English.toml" %SA_DIR%"\CheatMenuSA\locale\" /K /D /H /Y 
xcopy /s "..\resource\common\locale\English.toml" %VC_DIR%"\CheatMenuVC\locale\" /K /D /H /Y 
xcopy /s "..\resource\common\locale\English.toml" %III_DIR%"\CheatMenuIII\locale\" /K /D /H /Y 