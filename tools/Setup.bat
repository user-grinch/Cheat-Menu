rem Generate visual studio files and run the devcmd init
@echo off

rem Set game paths below
set "SA_DIR="E:\GTASanAndreas""
set "VC_DIR="F:\GTA Vice City""
set "III_DIR="F:\GTA3""

cd tools
premake5.exe vs2022
cd ../build
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat"
