#ifndef MEASUREMENT_SAVER_H
#define MEASUREMENT_SAVER_H

#include "MeasurementData.h"
#include "fstream"
#include "Constants.h"

class MeasurementSaver {

public:
	void SaveMeasurement(MeasurementData, bool, bool);
private:
};

#endif
