#include "WaveNode.h"

#ifndef MEASUREMENTDATA_H
#define MEASUREMENTDATA_H


#include <string>

struct MeasurementData {
	
	unsigned int id;
	
	std::string subject;
	std::string subtype; //example young, mature
	float weight; //Can be used as "health" of plant, mostly just used to weigh in how much to consider this
	bool healthy;
	
	int waveCount;
	WaveNode * waves;
	
};

#endif
