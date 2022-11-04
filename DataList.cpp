#include "DataList.h"
#include "MeasurementData.h"
#include <stdio.h>

DataList::DataList() {
	
}

void DataList::ReadWaveValues() {
	
	std::ifstream inFile;
	
	waveValues = new float[WAVELENGTH_AMOUNT];
	
	inFile.open("wavelength.txt");
	
	for(int i = 0; i < WAVELENGTH_AMOUNT; i++) {
		
		inFile >> waveValues[i];
		
	}
	
	inFile.close();
	
}

MeasurementData DataList::FormMeasurement(unsigned int id, float weight, std::string subject, std::string subtype, bool healthy) {
	mDataBuffer.id = id;
	mDataBuffer.weight = weight;
	mDataBuffer.subject = subject;
	mDataBuffer.subtype = subtype;
	mDataBuffer.healthy = healthy;
	mDataBuffer.waves = waves;
	mDataBuffer.waveCount = waveCount;
	
	return mDataBuffer;
}

void DataList::SaveRawData() {
	std::ofstream outFile;
	outFile.open("Data/output.txt");
	for(int i = 0; i < numberOfLines; i++) {
		outFile << stringList[i];
	}
	outFile.close();
}

void DataList::ReadInPixels(uint16_t * pixels, uint16_t pixelAmount) {
	
	numberOfLines = pixelAmount;
	
	stringList = new std::string[pixelAmount];
	
	for(int i = 0; i < pixelAmount; i++) {
		
		char str[25];
		
		sprintf(str, "%u", pixels[i]);
		stringList[i] = str;
		stringList[i].push_back('\n');
		
	}
	
	//Now read in the string data into wave nodes
	
	waveCount = pixelAmount;
	waves = new WaveNode[pixelAmount];
	for(int i = 0; i < pixelAmount; i++) {
		WaveNode * newNode = new WaveNode;
		newNode->wavelength = waveValues[i];
		newNode->intensity = std::stof(stringList[i]);
		waves[i] = (*newNode);
	}
	
}

void DataList::ReadInBytes(uint8_t * bytes) {
	
	numberOfLines = 0;
	for(int i = 0; i < MAX_BYTES; i++) {
		
		if(bytes[i] == '\n') {
			numberOfLines++;
		}
		
	}
	
	std::cout << "Number of lines: " << numberOfLines << std::endl;
	
	int offset = 0;
	bool newLine = false;
	//stringList = (std::string*)calloc(numberOfLines, sizeof(std::string));
	stringList = new std::string[numberOfLines];
	for(int i = 0; i < numberOfLines; i++) {
		
		std::string *newString = new std::string();
		newLine = false;
		while(!newLine) {
			//std::cout << bytes[offset];
			
			if(bytes[offset] == '\n') {
				newLine = true;
			}
			else {
				newString->push_back(bytes[offset]);
			}
			offset++;
		}
		stringList[i] = (*newString);
		//std::cout << "AFTER" << std::endl;
		
	}
	
	//Now read in the string data into wave nodes
	
	waves = new WaveNode[WAVELENGTH_AMOUNT];
	for(int i = WAVELENGTH_START; i < WAVELENGTH_AMOUNT; i++) {
		WaveNode * newNode = new WaveNode;
		newNode->wavelength = std::stof(stringList[i-1]);
		newNode->intensity = std::stof(stringList[(i-WAVELENGTH_START)+INTENSITY_START-1]);
		waves[i-WAVELENGTH_START] = (*newNode);
	}
	
}

void DataList::DisplayLines() {
	for(int i = 0; i < numberOfLines; i++) {
		std::cout << stringList[i] << std::endl;
	}
}

std::string DataList::GetLine(int i) {
	if(i < numberOfLines) {
		return stringList[i];
	}
	std::string output = "ERROR: Line index out of bounds. (";
	return output + std::to_string(i) + " > " + std::to_string(numberOfLines) + ")";
}
