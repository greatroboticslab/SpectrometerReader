@echo off

set loop=0
:loop
set /a loop=%loop%+1
"Spectrometer Reader.exe" -subject subject -subtype young -wavelength 400 -healthy yes -read -nobinary
if "%loop%"=="10" goto end
goto loop

:end
echo Finished.
pause