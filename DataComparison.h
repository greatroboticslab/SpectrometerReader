#ifndef DATA_COMPARISON_H
#define DATA_COMPARISON_H

#include "DataReader.h"
#include <windows.h>
#include <strsafe.h>
#include <vector>

class DataComparison {
	
public:
	
	int CompareData(MeasurementData &, float);
	
private:
	bool IsValidFile(std::string);
	bool WithinThreshold(MeasurementData &, MeasurementData &, float);
};

#endif
