@echo off
cd tools
premake5.exe vs2022
cd ../build
call "C:\Program Files\Microsoft Visual Studio\2022\Preview\Common7\Tools\VsDevCmd.bat"
MsBuild CheatMenu.sln /property:Configuration=Release /t:CheatMenuSA
cd ..
