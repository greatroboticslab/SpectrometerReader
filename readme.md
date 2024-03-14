# Measuring Data

### Single Read

Use command:

"Spectrometer Reader.exe" -subject <subject_name> -subtype <subtype_name> -wavelength <wavelength_nm> -healthy <yes/no> -read -iterations 1 -exposuretime 5000 -nobinary

### Batch Read

Run mass_read.bat. Reads in many readings over an amount of time.
Edit line 6 to arguments of your choosing.

# Reading Data

Readings are saved in the Data folder as a .csv file. You can view the readings by opening the .csv file with spectrograph_viewer.py (also in the Data folder). This can also be done by simply dragging the .csv folder on top of the python file.
