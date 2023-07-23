rem Generate visual studio files and run the devcmd init
@echo off

rem Set game paths below
set "SA_DIR="F:\GTA San Andreas""
set "VC_DIR="F:\GTA Vice City""
set "III_DIR="F:\GTA3""

cd tools
premake5.exe vs2022
cd ../build
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
