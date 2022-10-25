#ifndef DATAREADER_H
#define DATAREADER_H

#include "MeasurementData.h"
#include <fstream>
#include "stdint.h"
#include <sys/stat.h>
#include <iostream>
#include "Constants.h"

class DataReader {
	
public:
	
	DataReader();
	MeasurementData *ReadDataFile(std::string);
	
	//Array of measurements and its size are used as parameters
	int ReadReference(std::string, MeasurementData*&, int &);
	
private:
	
	
	
};

#endif
