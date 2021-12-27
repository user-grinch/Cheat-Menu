@echo off
echo --------------------------------------------------
echo Building DebugVC
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild CheatMenu.sln /property:Configuration=Debug /t:CheatMenuVC
