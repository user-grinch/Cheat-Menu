@echo off
echo --------------------------------------------------
echo Building ReleaseIII
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild CheatMenu.sln /property:Configuration=Release /t:CheatMenuIII
