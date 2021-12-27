@echo off
@REM Packs all game files into their proper archives (Yes I'm that lazy)

@REM ------------------------------------------------------
@REM Set game paths below
set "SA_DIR="F:\GTASanAndreas""
set "VC_DIR="E:\GTA Vice City""
set "III_DIR="E:\GTA3""
@REM ------------------------------------------------------

echo ------------------------------------------------------
echo "CheatMenu Packaging Utility"
echo ------------------------------------------------------
cd tools
echo Packing SA...
call :copyFiles %SA_DIR% "CheatMenuSA"
echo Packing VC...
call :copyFiles %VC_DIR% "CheatMenuVC"
echo Packing III...
call :copyFiles %III_DIR% "CheatMenuIII"
cd ..
rd /S /Q "pack" /Q
exit


@REM  ------------------------------------------------------
@REM  Functions
@REM  ------------------------------------------------------



:copyFiles
set "asiPath="%~1\%~2.asi""
set "folderpath="%~1\CheatMenu""
set "archivePath="..\%~2.7z""

@REM Remove existing files
rd /S /Q "pack" /Q
rd /S /Q %archivePath% /Q

@REM Copy the files to a temp folder
@REM straight xcopy doesn't seem to work on my system, broken registry?
%systemroot%\System32\xcopy /s %asiPath% "pack\" /K /D /H /Y 
%systemroot%\System32\xcopy /s %folderpath% "pack\CheatMenu\" /K /D /H /Y 

@REM Remove the config and versioninfo files since we don't want to distribute them
del "pack\CheatMenu\json\config.json" /Q
del "pack\CheatMenu\json\versioninfo.json" /Q

@REM Guessing we have 7zip installed already, well I have 
"C:\Program Files\7-Zip\7z.exe" a -t7z %archivePath% ".\pack\*"

@REM  ------------------------------------------------------
