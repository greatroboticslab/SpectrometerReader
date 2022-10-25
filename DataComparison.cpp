#include "DataComparison.h"
#include <iostream>

int DataComparison::CompareData(MeasurementData &m, float threshold) {
	
	DataReader dataReader;
	//MeasurementData reference = *(dataReader.ReadDataFile("BuiltData/worm.dat"));
	
	//similarity = 0;
	
	bool foundMatch = false; //Was an entry found that had a valid wavelength?
	
	std::vector<MeasurementData> measurements;
	
	//Loop through all files
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	TCHAR szDir[MAX_PATH];
	
	StringCchCopy(szDir, MAX_PATH, "BuiltData");
	StringCchCat(szDir, MAX_PATH, TEXT("/*"));
	
	std::cout << szDir << std::endl;
	
	hFind = FindFirstFile(szDir, &ffd);
	
	if(hFind == INVALID_HANDLE_VALUE) {
		std::cout << "No files found!" << std::endl;
	}
	
	int totalMeasurements = 0;
	
	do {
		
		std::string fileName = "BuiltData/";
		fileName += ffd.cFileName;
		std::cout << fileName << std::endl;
		if(IsValidFile(fileName)) {
		
			int mCount;
			MeasurementData *mData;
			dataReader.ReadReference(fileName, mData, mCount);
			std::cout << mCount << std::endl;
			
			for(int i = 0; i < mCount; i++) {
				measurements.push_back(mData[i]);
			}
			
			totalMeasurements += mCount;
			
		}
	}
	while(FindNextFile(hFind, &ffd) != 0);
	
	//Iterate through all measurements, and return the most similar measurement
	int *similarity;
	similarity = new int[totalMeasurements];
	
	for(int i = 0; i < totalMeasurements; i++) {
		
		//Print entry
		//std::cout << measurements[i].subject << ":" << measurements[i].subtype << ":" << measurements[i].healthy << std::endl;
		
		similarity[i] = 0;
		
		for(int j = 0; j < measurements[i].waveCount; j++) {
			
			float am = m.waves[j].intensity - measurements[i].waves[j].intensity;
			if(am < 0) {
				am *= -1.0;
			}
			
			//Print Waves
			//std::cout << "Wave " << measurements[i].waves[j].wavelength << ", " << measurements[i].waves[j].intensity << std::endl;
			
			similarity[i] += am;
			
		}
		
	}
	
	
	//IMPORTANT, THE SECOND ONE MIGHT HAVE INCORRECT WAVE DATA, PRINT OUT AND CHECK!!
	
	//Get index of most similar measurement
	int highest = 0;
	for(int i = 0; i < totalMeasurements; i++) {
		
		//Valid wavelength?
		if(WithinThreshold(m,measurements[i], threshold)) {
			
			foundMatch = true;
			std::cout << i << "): " << similarity[i] << std::endl;
		
			if(similarity[i] < similarity[highest]) {
				highest = i;
			}
		}	
	}
	
	if(foundMatch) {
	
		//Print out most similar measurement
		std::cout << "This measurement most closely matches: " << std::endl;
		MeasurementData md = measurements[highest];
		std::cout << "=======================================" << std::endl;
		std::cout << "Subject: " << md.subject << std::endl;
		std::cout << "Subtype: " << md.subtype << std::endl;
		std::cout << "Wavelength: " << md.weight << std::endl;
		std::cout << "Healthy: " << md.healthy << std::endl;
		std::cout << "=======================================" << std::endl;
		
	}
	
	else {
		
		std::cout << "No valid entries found! (No measurements with a matching wavelength were taken." << std::endl;
		
	}
	
	return 0;
	
}

bool DataComparison::WithinThreshold(MeasurementData &measurement, MeasurementData &entry, float threshold) {
	
	if(measurement.weight >= entry.weight - threshold && measurement.weight <= entry.weight + threshold) {
		return true;
	}
	return false;
}

bool DataComparison::IsValidFile(std::string fileName) {
	
	if(fileName.size() > 5) {
		std::string extension = fileName.substr(fileName.size()-4, 4);
		if(extension == ".dat") {
			return true;
		}
	}
	return false;
}
