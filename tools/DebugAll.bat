@echo off
echo --------------------------------------------------
echo Building DebugAll
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild CheatMenu.sln /property:Configuration=Debug
