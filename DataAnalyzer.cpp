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
	
	//std::cout << szDir << std::endl;
	
	hFind = FindFirstFile(szDir, &ffd);
	
	if(hFind == INVALID_HANDLE_VALUE) {
		std::cout << "No files found!" << std::endl;
	}
	
	do {
		std::string fileName = "Data/";
		fileName += ffd.cFileName;
		//std::cout << fileName << std::endl;
		if(IsValidFile(fileName)) {
			
			std::cout << fileName << std::endl;
			
			DataReader dataReader;
			MeasurementData currentMeasurement;
			currentMeasurement = *(dataReader.ReadDataFile(fileName));
			
			//If there is a matching data type
			if(SubjectMatches(currentMeasurement)) {
				
				//Modify data
				ModifyEntry(currentMeasurement);
				
			}
			
			//No matching data type
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
		
		//saver.SaveMeasurement(outputData[i], true, true);
		
		
	}
	
	std::cout << "MADE IT HERE" << std::endl;
	
	//Now finally construct the references
	std::vector<std::string> subjectStrings;
	int b;
	
	int rCount = 0;
	
	for(int i = 0; i < outputData.size(); i++) {
		
		if(ReferenceExists(outputData[i].subject, subjectStrings, b)) {
			
			/*
			MeasurementData newData = new MeasurementData();
			newData.subject = outputData.subject;
			newData.subtype = outputData.subtype;
			newData.waveCount = outputData.waveCount;
			newData.waves = outputData.waves;
			newData.weight = outputData.weight;
			newData.healthy = outputData.healthy;*/
			references[b].measurements.push_back(outputData[i]);
			
		}
		else {
			
			ReferenceData newRef;
			references.push_back(newRef);
			subjectStrings.push_back(outputData[i].subject);
			references[rCount].measurements.push_back(outputData[i]);
			rCount++;
			
		}
		
		
		
	}
	
	MeasurementSaver saver;
	//Now save all the data
	
	for(int i = 0; i < references.size(); i++) {
		saver.SaveReference(references[i]);
		
	}
	
}

bool DataAnalyzer::ReferenceExists(std::string name, std::vector<std::string> subjectStrings, int &id) {
	
	for(int i = 0; i < subjectStrings.size(); i++) {
		
		//std::cout << "MOMET: " << subjectStrings[i] << std::endl;
		
		if(subjectStrings[i] == name) {
			id = i;
			return true;
		}
		
	}
	return false;
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
			if(outputData[i].healthy == m.healthy) {
				return true;
			}
		}
	}
	return false;
}

bool DataAnalyzer::SubjectExists(MeasurementData &m) {
	
	for(int i = 0; i < outputData.size(); i++) {
		if(outputData[i].subject == m.subject) {
			return true;
		}
	}
	return false;
	
}

bool DataAnalyzer::SubjectMatches(MeasurementData &m) {
	
	for(int i = 0; i < outputData.size(); i++) {
		if(outputData[i].subject == m.subject) {
			if(outputData[i].healthy == m.healthy) {
				if(outputData[i].subtype == m.subtype) {
					if(outputData[i].weight == m.weight) {
						//std::cout << "SUBJECT MATCHES!" << std::endl;
						return true;
					}
				}
				
			}
		}
	}
	return false;
	
}

void DataAnalyzer::ModifyEntry(MeasurementData &m) {
	
	for(int i = 0; i < outputData.size(); i++) {
		if(SubjectMatches(m)) {
				
			for(int z = 0; z < outputData[i].waveCount; z++) {
				
				outputData[i].waves[z].intensity += m.waves[z].intensity;
				
			}
			
			outputCount[i]++;
			
		}
	}
	
}
