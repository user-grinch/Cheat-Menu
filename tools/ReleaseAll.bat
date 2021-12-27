@echo off
echo --------------------------------------------------
echo Building ReleaseAll
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild CheatMenu.sln /property:Configuration=Release
