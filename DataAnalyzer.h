#ifndef DATA_ANALYZER_H
#define DATA_ANALYZER_H

#include <string>
#include "MeasurementData.h"
#include "MeasurementSaver.h"
#include "DataList.h"
#include <vector>

class DataAnalyzer {
	
public:
	void BuildData();
	DataAnalyzer();
	
private:
	bool IsValidFile(std::string);
	bool DataExists(MeasurementData&);
	std::vector<MeasurementData> outputData;
	std::vector<int> outputCount;
	void ModifyEntry(MeasurementData&);
};

#endif
