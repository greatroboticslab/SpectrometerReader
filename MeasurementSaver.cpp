#include "MeasurementSaver.h"

void MeasurementSaver::SaveMeasurement(MeasurementData mData, bool frameMode, bool final) {
	
	std::string fileName;
	
	if(final) {
		fileName = mData.subject + ".dat";
	}
	else {
		fileName = std::to_string(mData.id);
		fileName += "_" + mData.subject + ".dat";
	}
	std::ofstream outFile;
	
	if(final) {
		outFile.open("BuiltData/"+fileName, std::ios::out | std::ios::binary);
	}
	else {
		outFile.open("Data/"+fileName, std::ios::out | std::ios::binary);
	}
	
	
	
	//File size = subject + subject size (byte) + subtype + sybtype size (byts) + wavenodes*floatsize*2
	int fileSize = sizeof(float)*2*WAVELENGTH_AMOUNT;
	fileSize += mData.subject.size();
	fileSize += mData.subtype.size();
	unsigned char subjectSize = mData.subject.size();
	unsigned char subtypeSize = mData.subtype.size();
	fileSize += 2; //Size bytes for strings;
	fileSize += sizeof(mData.weight);
	uint8_t *bytes = new uint8_t[fileSize];
	
	int i = 0;
	
	bytes[0] = subjectSize;
	bytes[1] = subtypeSize;
	
	i += 2;
	for(int j = 0; j < mData.subject.size(); j++) {
		bytes[i] = mData.subject[j];
		i++;
	}
	for(int j = 0; j < mData.subtype.size(); j++) {
		bytes[i] = mData.subtype[j];
		i++;
	}
	
	//Now include weight
	unsigned char const * bi = reinterpret_cast<unsigned char const *>(&mData.weight);
	
	bytes[i] = bi[0];
	bytes[i+1] = bi[1];
	bytes[i+2] = bi[2];
	bytes[i+3] = bi[3];
	i+=4;
	
	for(int j = 0; j < WAVELENGTH_AMOUNT; j++) {
		
		unsigned char const * b = reinterpret_cast<unsigned char const *>(&mData.waves[j].wavelength);
		
		bytes[i] = b[0];
		bytes[i+1] = b[1];
		bytes[i+2] = b[2];
		bytes[i+3] = b[3];
		
		b = reinterpret_cast<unsigned char const *>(&mData.waves[j].intensity);
		int offset = WAVELENGTH_AMOUNT*4;
		
		bytes[offset+i] = b[0];
		bytes[offset+i+1] = b[1];
		bytes[offset+i+2] = b[2];
		bytes[offset+i+3] = b[3];
		
		i+=4;
	}
	//std::cout << fileSize << " bytes compared to " << i*2 << std::endl;
	
	outFile.write((char*)bytes, fileSize);
	delete [] bytes;
	
	outFile.close();
	
}
