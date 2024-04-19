# Project Objective

This repo allows the collection of spectrometer readings in the form of .csv files to be trained to identify plants and their characteristics.

# Dependencies

spectrlib_shared_64bits.dll is included but must be in the same directory as the executable.

# Command Line

The file "Spectrometer Reader.exe" handles the reading from the spectrometer, and can take several arguments:

"Spectrometer Reader.exe" -subject <subject_name> -subtype <subtype_name> -wavelength <wavelength_nm> -healthy <yes/no> -read -iterations 1 -exposuretime 5000 -nobinary

# Measuring Data

Once you have your ASEQ spectrometer connected, you can make measurements. These measurements will be stored in the Data\ directory.

### Single Read

Use command:

"Spectrometer Reader.exe" -subject <subject_name> -subtype <subtype_name> -wavelength <wavelength_nm> -healthy <yes/no> -read -iterations 1 -exposuretime 5000 -nobinary

For example:

"Spectrometer Reader.exe" -subject maple_leaf -subtype mature -wavelength 1000 -healthy yes -read -iterations 1 -exposuretime 5000 -nobinary

### Batch Read

Run mass_read.bat. Reads in many readings over an amount of time. This is accomplished by using a batch for loop that simply executes the Spectrometer Reader.exe command with arguments over and over again.
Edit line 6 to arguments of your choosing.

By default, it will make 1 recording. To do more than 1 recording, edit the number "1" in the line

if "%loop%"=="1" goto end

to a number of your choosing.

# Visualization Data

Readings are saved in the Data folder as a .csv file. You can view the readings by opening the .csv file with spectrograph_viewer.py (also in the Data folder). This can also be done by simply dragging the .csv folder on top of the python file.

# Troubleshooting

- Make sure the ASEQ spectrometer is plugged into the computer, and that the required .dll is in the same directory as the executable.
- Make sure you enter the arguments correctly as defined in the Single Read section.
- You can run just the command "Spectrometer Reader.exe" and type help for a list of commands. This was the old way of running the program and is deprecated, but if you type status in this mode it will show if a spectrometer is connected and recognized.
