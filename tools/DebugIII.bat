@echo off
echo --------------------------------------------------
echo Building DebugIII
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild CheatMenu.sln /property:Configuration=Debug /t:CheatMenuIII
del %III_DIR%"\CheatMenuIII.asi" /Q
del %III_DIR%"\CheatMenuIII.pdb" /Q
xcopy /s "bin\CheatMenuIII.asi" %III_DIR% /K /D /H /Y 
xcopy /s "bin\CheatMenuIII.pdb" %III_DIR% /K /D /H /Y 
