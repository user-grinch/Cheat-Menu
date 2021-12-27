@echo off
echo --------------------------------------------------
echo Building ReleaseVC
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild CheatMenu.sln /property:Configuration=Release /t:CheatMenuVC
