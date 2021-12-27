@echo off
echo --------------------------------------------------
echo Building DebugIII
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild CheatMenu.sln /property:Configuration=Debug /t:CheatMenuIII
