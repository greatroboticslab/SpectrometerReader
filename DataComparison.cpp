#include "DataComparison.h"
#include <iostream>

int DataComparison::CompareData(MeasurementData &m, std::string subject, int &similarity) {
	
	DataReader dataReader;
	MeasurementData reference = *(dataReader.ReadDataFile("BuiltData/"+subject+".dat"));
	
	similarity = 0;
	
	for(int i = 0; i < reference.waveCount; i++) {
		
		similarity += m.waves[i].intensity - reference.waves[i].intensity;
		
		std::cout << m.waves[i].intensity - reference.waves[i].intensity << std::endl;
		
	}
	
	return 0;
	
}
