@echo off
@REM Packs all game files into their proper archives (Yes I'm that lazy)

echo ------------------------------------------------------
echo "CheatMenu Packaging Utility"
echo ------------------------------------------------------
rd /S /Q "archive" /Q
cd tools
echo Packing SA...
call :copyFiles "CheatMenuSA"
echo Packing VC...
call :copyFiles "CheatMenuVC"
echo Packing III...
call :copyFiles "CheatMenuIII"
rd /S /Q "pack" /Q
cd ..
exit


@REM  ------------------------------------------------------
@REM  Functions
@REM  ------------------------------------------------------



:copyFiles
set "asiPath="..\build\bin\%~1.asi""
set "folderpath="..\resource\%~1""
set "archivePath="..\archive\%~1.7z""

@REM Remove existing files
rd /S /Q "pack" /Q
rd /S /Q %archivePath% /Q

@REM Copy the files to a temp folder
@REM straight xcopy doesn't seem to work on my system, broken registry?
%systemroot%\System32\xcopy /s %asiPath% "pack\" /K /D /H /Y 
%systemroot%\System32\xcopy /s %folderpath% "pack\CheatMenu\" /K /D /H /Y 
%systemroot%\System32\xcopy /s "..\resource\common\" "pack\CheatMenu\" /K /D /H /Y

@REM Remove the config and versioninfo files since we don't want to distribute them
del "pack\CheatMenu\json\config.json" /Q
del "pack\CheatMenu\json\versioninfo.json" /Q

@REM Guessing we have 7zip installed already, well I have 
"C:\Program Files\7-Zip\7z.exe" a -t7z %archivePath% ".\pack\*"

@REM  ------------------------------------------------------
