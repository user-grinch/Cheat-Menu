@echo off
echo --------------------------------------------------
echo Building ReleaseIII
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild CheatMenu.sln /property:Configuration=Release /t:CheatMenuIII
del %III_DIR%"\CheatMenuIII.asi" /Q
xcopy /s "bin\CheatMenuIII.asi" %III_DIR% /K /D /H /Y 
