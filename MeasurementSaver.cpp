#include "MeasurementSaver.h"
#include <iostream>

void MeasurementSaver::SaveCSV(MeasurementData *m, std::string name, bool fromFile, bool rawmes) {
	
	std::ofstream outFile;
	
	if(fromFile) {
		outFile.open("Data/"+name+".csv");
	}
	else {
		std::string fileName;
		
		fileName = "";
		if(!rawmes) {
			fileName = std::to_string(m->id);
			fileName += "_";
			fileName += m->subject + ".csv";
		}
		else {
			fileName = "spectrum.csv";
		}
		
		outFile.open("Data/"+fileName);
	}
	
	outFile << "Subject,Subtype,Healthy,Sent Wavelength,Returned Wavelength,Returned Intensity\n";
	
	for(int i = 0; i < m->waveCount; i++) {
		
		std::string line = m->subject;
		line += "," + m->subtype;
		line += ",";
		if(m->healthy) {
			line+= "yes";
		}
		else {
			line+= "no";
		}
		line += "," + std::to_string(m->weight);
		line += "," + std::to_string(m->waves[i].wavelength);
		line += "," + std::to_string(m->waves[i].intensity);
		line += "\n";
		outFile << line;
		
	}
	
	outFile.close();
	
}

void MeasurementSaver::SaveMeasurement(MeasurementData mData, bool frameMode) {
	
	std::string fileName;
	
	fileName = std::to_string(mData.id);
	fileName += "_" + mData.subject + ".dat";
	
	std::ofstream outFile;
	
	outFile.open("Data/"+fileName, std::ios::out | std::ios::binary);
		
	bytes = new uint8_t[GetFSize(mData)];
	BuildBytes(mData, 0, true);
	
	//File size = subject + subject size (byte) + subtype + sybtype size (byts) + wavenodes*floatsize*2
	
	//std::cout << fileSize << " bytes compared to " << i*2 << std::endl;
	
	outFile.write((char*)bytes, GetFSize(mData));
	delete [] bytes;
	
	outFile.close();
	
}

//Get list of compiled entries, save them to a single file
void MeasurementSaver::SaveReference(ReferenceData reference) {
	
	//Get file size
	
	int fileSize = 0;
	
	//Number of entries
	fileSize += sizeof(int);
	
	//HOW DATA IS BUILT
	
	//# of entries
	/* {filesize
		data
	
	}*/
	
	std::cout << "COUNT " << reference.measurements.size() << std::endl;
	for(int k = 0; k < reference.measurements.size(); k++) {
		
		fileSize += sizeof(int);
		fileSize += GetFSize(reference.measurements[k]);
		
	}
	
	bytes = new uint8_t[fileSize];
	int iter = 0; //Index position of byte
	
	int mCount = reference.measurements.size();
	
	unsigned char const * zi = reinterpret_cast<unsigned char const *>(&mCount);
	
	std::cout << "FS: " << fileSize << ", " << sizeof(mCount) << std::endl;
	
	
	bytes[iter] = zi[0];
	bytes[iter+1] = zi[1];
	bytes[iter+2] = zi[2];
	bytes[iter+3] = zi[3];
	
	iter += 4; //Bytepos
	
	for(int i = 0; i < mCount; i++) {
		
		//Start with filesize
		
		int entrySize = GetFSize(reference.measurements[i]);
		
		
		//DATA IS READ IN CORRECTLY FROM REFERENCE
		//Print entry
		/*
		std::cout << reference.measurements[i].subject << ":" << reference.measurements[i].subtype << ":" << reference.measurements[i].healthy << std::endl;
		for(int k = 0; k < reference.measurements[i].waveCount; k++) {
			
			std::cout << "Wave " << k << ": " << reference.measurements[i].waves[k].intensity << std::endl;
			
		}
		*/
		
		unsigned char const * bi = reinterpret_cast<unsigned char const *>(&entrySize);
	
		bytes[iter] = bi[0];
		bytes[iter+1] = bi[1];
		bytes[iter+2] = bi[2];
		bytes[iter+3] = bi[3];
		
		iter += 4;
		
		BuildBytes(reference.measurements[i], iter, true);
		
		iter += entrySize;
		
		std::cout << "HERE" << std::endl;
	}
	
	std::cout << "FILESIZE: " << fileSize << std::endl;
	
	std::string fileName;
	fileName = reference.measurements[0].subject + ".dat";
	
	std::ofstream outFile;
	
	outFile.open("BuiltData/"+fileName, std::ios::out | std::ios::binary);
	
	outFile.write((char*)bytes, fileSize);
	
	outFile.close();
	
	delete [] bytes;
	
}

int MeasurementSaver::GetFSize(MeasurementData &mData) {
	
	int fileSize = sizeof(float)*2*WAVELENGTH_AMOUNT;
	fileSize += mData.subject.size();
	fileSize += mData.subtype.size();
	unsigned char subjectSize = mData.subject.size();
	unsigned char subtypeSize = mData.subtype.size();
	fileSize += 2; //Size bytes for strings;
	fileSize += sizeof(mData.weight);
	//bytes = new uint8_t[fileSize];
	
	//Size of bool (healthy)
	fileSize += sizeof(bool);
	
	return fileSize;
	
}

void MeasurementSaver::BuildBytes(MeasurementData &mData, int offset, bool frameMode) {
	
	int fileSize = GetFSize(mData);
	
	int i = 0 + offset;
	
	unsigned char subjectSize = mData.subject.size();
	unsigned char subtypeSize = mData.subtype.size();
	
	//String sizes
	bytes[i] = subjectSize;
	bytes[i+1] = subtypeSize;
	
	//Healthy
	bytes[i+2] = mData.healthy;
	
	i += 3;
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
	
}
