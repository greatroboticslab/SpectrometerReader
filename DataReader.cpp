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
	
	mData->healthy = bytes[2];
	
	std::cout << "Filesize: " << fileSize << std::endl;
	//std::cout << "Subject size: " << subjectSize << std::endl;
	//std::cout << "Subtype size: " << subtypeSize << std::endl;
	
	std::string subject, subtype;
	subject = subtype = "";
	
	int i = 3;
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

// Read a reference file

int DataReader::ReadReference(std::string fileName, MeasurementData*& mData, int &count) {
	
	std::ifstream inFile;
	inFile.open(fileName, std::ios::binary);
	
	int iter = 0;
	
	struct stat results;
	stat((fileName).c_str(), &results);
	int fileSize = results.st_size;
	uint8_t * bytes = new uint8_t[fileSize];
	inFile.read((char*)bytes,fileSize);
	
	//Get number of measurements
	
	unsigned char fBuf[sizeof(float)];
	fBuf[0] = bytes[iter];
	fBuf[1] = bytes[iter+1];
	fBuf[2] = bytes[iter+2];
	fBuf[3] = bytes[iter+3];
	iter+= 4;
	
	count = (*(int*)fBuf);
	
	//std::cout << "NEWCOUNT: " << count << std::endl;
	
	mData = new MeasurementData[count];
	
	for(int i = 0; i < count; i++) {
		
		int fSize;
		
		unsigned char sBuf[sizeof(float)];
		sBuf[0] = bytes[iter];
		sBuf[1] = bytes[iter+1];
		sBuf[2] = bytes[iter+2];
		sBuf[3] = bytes[iter+3];
		iter+= 4;
		
		fSize = (*(int*)sBuf);
		
		int subjectSize = bytes[iter];
		int subtypeSize = bytes[iter+1];
		
		//std::cout << "Subject Size: " << subjectSize << std::endl;
		//std::cout << "Subtype Size: " << subtypeSize << std::endl;
		
		mData[i].healthy = bytes[iter+2];
		//std::cout << "Subject size: " << subjectSize << std::endl;
		//std::cout << "Subtype size: " << subtypeSize << std::endl;
		
		std::string subject, subtype;
		subject = subtype = "";
		
		iter += 3;
		for(int j = 0; j < subjectSize; j++) {
			subject += bytes[iter];
			iter++;
		}
		for(int j = 0; j < subtypeSize; j++) {
			subtype += bytes[iter];
			iter++;
		}
		
		mData[i].subject = subject;
		mData[i].subtype = subtype;
		
		//std::cout << subject << std::endl;
		
		float weight;
		unsigned char fBuf[sizeof(float)];
		fBuf[0] = bytes[iter];
		fBuf[1] = bytes[iter+1];
		fBuf[2] = bytes[iter+2];
		fBuf[3] = bytes[iter+3];
		iter+= 4;
		
		weight = (*(float*)fBuf);
		
		//std::cout << "Weight: " << weight << std::endl;
		
		mData[i].weight = weight;
		
		int waveCount = WAVELENGTH_AMOUNT;
		mData[i].waveCount = waveCount;
		mData[i].waves = new WaveNode[waveCount];
		//i-=1;
		
		for(int j = 0; j < WAVELENGTH_AMOUNT; j++) {
			
			float wavelength;
			float intensity;
			
			//Wavelength
			unsigned char wBuf[sizeof(float)];
			wBuf[0] = bytes[iter];
			wBuf[1] = bytes[iter+1];
			wBuf[2] = bytes[iter+2];
			wBuf[3] = bytes[iter+3];
			
			wavelength = (*(float*)wBuf);
			
			unsigned char iBuf[sizeof(float)];
			
			int offset = WAVELENGTH_AMOUNT*4;
			
			iBuf[0] = bytes[offset+iter];
			iBuf[1] = bytes[offset+1+iter];
			iBuf[2] = bytes[offset+2+iter];
			iBuf[3] = bytes[offset+3+iter];
			
			intensity = (*(float*)iBuf);
			
			//std::cout << "Wavelength: " << "(" << i << ")" << wavelength << ", Intensity: " << "(" << offset << ")" << intensity << std::endl;
			
			mData[i].waves[j].wavelength = wavelength;
			mData[i].waves[j].intensity = intensity;
			
			iter += sizeof(float);
			
		}
		
		
		int offset = i+WAVELENGTH_AMOUNT*4;
		iter += offset;
		//std::cout << "FILEBUF: " << iter << "/" << fileSize << std::endl;
		
	}
	
	/*
	MeasurementData *mData = new MeasurementData();
	
	struct stat results;
	stat((fileName).c_str(), &results);
	int fileSize = results.st_size;
	uint8_t * bytes = new uint8_t[fileSize];
	inFile.read((char*)bytes,fileSize);
	
	int subjectSize = bytes[0];
	int subtypeSize = bytes[1];
	
	mData->healthy = bytes[2];
	
	std::cout << "Filesize: " << fileSize << std::endl;
	//std::cout << "Subject size: " << subjectSize << std::endl;
	//std::cout << "Subtype size: " << subtypeSize << std::endl;
	
	std::string subject, subtype;
	subject = subtype = "";
	
	int i = 3;
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
	*/
	
	delete [] bytes;
	return 0;
	
}
