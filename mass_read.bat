@echo off

set loop=0
:loop
set /a loop=%loop%+1
"Spectrometer Reader.exe" -subject soybean_leaf -subtype young -wavelength 0 -healthy yes -read -iterations 1 -exposuretime 5000 -nobinary
if "%loop%"=="1" goto end
goto loop

:end
echo Finished.

FOR /F "eol=| delims=" %%I IN ('DIR "Data\*.csv" /A-D /B /O-D /TW 2^>nul') DO SET "NewestFile=%%I" & GOTO FoundFile
ECHO No *.jpg file found!
GOTO :EOF

:FoundFile
python Data\spectrograph_viewer.py Data\%NewestFile%
echo 
choice /C:YN /M "Keep this file? [Y/N]"
IF ERRORLEVEL ==2 GOTO deleteFile
IF ERRORLEVEL ==1 GOTO exeet

GOTO FoundFile

:deleteFile
del /f Data\%NewestFile%

:exeet
GOTO next

:next