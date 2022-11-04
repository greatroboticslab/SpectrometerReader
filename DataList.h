#ifndef DATALIST_H
#define DATALIST_H

#include <string>
#include "stdint.h"
#include "Constants.h"
#include "WaveNode.h"
#include <iostream>
#include <fstream>
#include "MeasurementData.h"

class DataList {
	
public:
	
	DataList();
	void ReadWaveValues();
	void ReadInBytes(uint8_t * bytes);
	void DisplayLines();
	std::string GetLine(int);
	void SaveRawData();
	MeasurementData FormMeasurement(unsigned int, float, std::string, std::string, bool);
	void ReadInPixels(uint16_t * pixels, uint16_t pixelAmount);
	
private:
	
	std::string * stringList;
	float * waveValues;
	int numberOfLines;
	WaveNode * waves;
	int waveCount;
	std::string subject, state;
	MeasurementData mDataBuffer;
	
};

#endif
