#include "DataReader.h"

DataReader::DataReader() {
	
	
	
}

MeasurementData* DataReader::ReadDataFile(std::string fileName) {
	
	std::ifstream inFile;
	inFile.open(fileName, std::ios::binary);
	
	MeasurementData *mData = new MeasurementData();
	
	struct stat results;
	stat((fileName).c_str(), &results);
	int fileSize = results.st_size;
	uint8_t * bytes = new uint8_t[fileSize];
	inFile.read((char*)bytes,fileSize);
	
	int subjectSize = bytes[0];
	int subtypeSize = bytes[1];
	
	std::cout << "Filesize: " << fileSize << std::endl;
	//std::cout << "Subject size: " << subjectSize << std::endl;
	//std::cout << "Subtype size: " << subtypeSize << std::endl;
	
	std::string subject, subtype;
	subject = subtype = "";
	
	int i = 2;
	for(int j = 0; j < subjectSize; j++) {
		subject += bytes[i];
		i++;
	}
	for(int j = 0; j < subtypeSize; j++) {
		subtype += bytes[i];
		i++;
	}
	
	mData->subject = subject;
	mData->subtype = subtype;
	
	float weight;
	unsigned char fBuf[sizeof(float)];
	fBuf[0] = bytes[i];
	fBuf[1] = bytes[i+1];
	fBuf[2] = bytes[i+2];
	fBuf[3] = bytes[i+3];
	i+= 4;
	
	weight = (*(float*)fBuf);
	
	std::cout << "Weight: " << weight << std::endl;
	
	mData->weight = weight;
	
	int waveCount = WAVELENGTH_AMOUNT;
	mData->waveCount = waveCount;
	mData->waves = new WaveNode[waveCount];
	//i-=1;
	
	for(int j = 0; j < WAVELENGTH_AMOUNT; j++) {
		
		float wavelength;
		float intensity;
		
		//Wavelength
		unsigned char wBuf[sizeof(float)];
		wBuf[0] = bytes[i];
		wBuf[1] = bytes[i+1];
		wBuf[2] = bytes[i+2];
		wBuf[3] = bytes[i+3];
		
		wavelength = (*(float*)wBuf);
		
		unsigned char iBuf[sizeof(float)];
		
		int offset = i+WAVELENGTH_AMOUNT*4;
		
		iBuf[0] = bytes[offset];
		iBuf[1] = bytes[offset+1];
		iBuf[2] = bytes[offset+2];
		iBuf[3] = bytes[offset+3];
		
		intensity = (*(float*)iBuf);
		
		//std::cout << "Wavelength: " << "(" << i << ")" << wavelength << ", Intensity: " << "(" << offset << ")" << intensity << std::endl;
		
		mData->waves[j].wavelength = wavelength;
		mData->waves[j].intensity = intensity;
		
		i+= sizeof(float);
		
	}
	
	delete [] bytes;
	
	inFile.close();
	
	return mData;
	
}
