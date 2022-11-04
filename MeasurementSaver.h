#ifndef MEASUREMENT_SAVER_H
#define MEASUREMENT_SAVER_H

#include "MeasurementData.h"
#include "ReferenceData.h"
#include "fstream"
#include "Constants.h"
#include <vector>

class MeasurementSaver {

public:
	void SaveMeasurement(MeasurementData, bool);
	void SaveReference(ReferenceData);
	void SaveCSV(MeasurementData*, std::string, bool);
private:
	uint8_t *bytes;
	void BuildBytes(MeasurementData&, int, bool);
	int GetFSize(MeasurementData&);
};

#endif
