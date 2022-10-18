#include "DataAnalyzer.h"
#include <windows.h>
#include <strsafe.h>
#include <iostream>
#include "DataReader.h"
#include <iostream>


DataAnalyzer::DataAnalyzer() {
	
}

void DataAnalyzer::BuildData() {
	
	//Loop through all files
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	TCHAR szDir[MAX_PATH];
	
	StringCchCopy(szDir, MAX_PATH, "Data");
	StringCchCat(szDir, MAX_PATH, TEXT("/*"));
	
	std::cout << szDir << std::endl;
	
	hFind = FindFirstFile(szDir, &ffd);
	
	if(hFind == INVALID_HANDLE_VALUE) {
		std::cout << "No files found!" << std::endl;
	}
	
	do {
		
		std::string fileName = "Data/";
		fileName += ffd.cFileName;
		std::cout << fileName << std::endl;
		if(IsValidFile(fileName)) {
			
			DataReader dataReader;
			MeasurementData currentMeasurement;
			currentMeasurement = *(dataReader.ReadDataFile(fileName));
			
			if(DataExists(currentMeasurement)) {
				
				//Modify data
				ModifyEntry(currentMeasurement);
				
			}
			else {
				
				//Add new entry
				MeasurementData * newMeasurement;
				newMeasurement = new MeasurementData();
				*(newMeasurement) = currentMeasurement;
				outputData.push_back(*newMeasurement);
				outputCount.push_back(1);
				
			}
			
		}
	}
	while(FindNextFile(hFind, &ffd) != 0);
	
	//Now save all the final measurements, divide them too
	for(int i = 0; i < outputData.size(); i++) {
		
		for(int w = 0; w < outputData[i].waveCount; w++) {
			
			outputData[i].waves[w].intensity /= outputCount[i];
			//std::cout << "Intensity: " << outputData[i].waves[w].intensity << ", Count: " << outputCount[i] << std::endl;
			
		}
		
		MeasurementSaver saver;
		saver.SaveMeasurement(outputData[i], true, true);
		
		
	}
	
	
}

bool DataAnalyzer::IsValidFile(std::string fileName) {
	
	if(fileName.size() > 5) {
		std::string extension = fileName.substr(fileName.size()-4, 4);
		if(extension == ".dat") {
			return true;
		}
	}
	return false;
}

bool DataAnalyzer::DataExists(MeasurementData &m) {
	
	for(int i = 0; i < outputData.size(); i++) {
		if(outputData[i].subject == m.subject) {
			return true;
		}
	}
	return false;
}

void DataAnalyzer::ModifyEntry(MeasurementData &m) {
	
	for(int i = 0; i < outputData.size(); i++) {
		if(outputData[i].subject == m.subject) {
			
			for(int z = 0; z < outputData[i].waveCount; z++) {
				
				outputData[i].waves[z].intensity += m.waves[z].intensity;
				
			}
			
			outputCount[i]++;
			
		}
	}
	
}
