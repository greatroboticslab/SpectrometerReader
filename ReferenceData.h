#ifndef REFERENCE_DATA_H
#define REFERENCE_DATA_H

#include "MeasurementData.h"
#include <vector>

struct ReferenceData{
	
	int count;
	std::vector<MeasurementData> measurements;
	
};

#endif
