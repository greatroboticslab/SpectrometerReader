#include <iostream>
#include <windows.h>
#include "stdint.h"
#include "errorCodes.h"
#include "Constants.h"
#include "DataList.h"
#include "DataReader.h"
#include "DataAnalyzer.h"
#include "DataComparison.h"

struct DeviceInfo_t {
	
	char *R; //Serial number
	DeviceInfo_t *next; //Next one
	
};

typedef int (*_clearMemory) (void);
_clearMemory clearMemory;

typedef int (*_getDeviceCount) (void);
_getDeviceCount getDeviceCount;

typedef int (*_getStatus) (uint8_t*, uint16_t *);
_getStatus getStatus;

typedef int (*_connectToDeviceByIndex) (unsigned int);
_connectToDeviceByIndex connectToDeviceByIndex;

typedef DeviceInfo_t * (*_getDevicesInfo) (void);
_getDevicesInfo getDevicesInfo;

typedef int (*_resetDevice) (void);
_resetDevice resetDevice;

typedef int (*_triggerAcquisition) (void);
_triggerAcquisition triggerAcquisition;

typedef int (*_readFlash) (uint8_t *, uint32_t, const uint16_t);
_readFlash readFlash;

typedef int (*_eraseFlash) (void);
_eraseFlash eraseFlash;

typedef int (*_getFrameFormat) (const uint16_t*, const uint16_t*, const uint8_t*, uint16_t*);
_getFrameFormat getFrameFormat;

typedef int (*_getFrame) (uint16_t*, const uint16_t);
_getFrame getFrame;

HINSTANCE hinst;

bool LoadDLL();

void ReadData(uint8_t*);

unsigned int ReadID();

MeasurementData MakeReading(unsigned int &id, DataList &dataList, std::string subject);

void SaveMeasurement(MeasurementData &mData, bool);

void SaveID(unsigned int);

unsigned char statusFlags;
unsigned short framesInMemory;
DeviceInfo_t * device;


int main() {
	
	unsigned int currentID = ReadID();
	
	DataList dataList;
	DataReader dataReader;
	
	if(!LoadDLL()) {
		std::cout << "Could not locate the required .dll file (spectrlib_shared_64bits.dll)" << std::endl;
		return ERROR_NO_DLL;
	}
	
	//PUT STUFF AFTER LOADING DLL
	
	
	
	
	
	
	if(getDeviceCount() == 0) {
		std::cout << "No spectrometer detected! Try running again after pluggin in the spectrometer." << std::endl;
		return ERROR_NOT_CONNECTED;
	}
	connectToDeviceByIndex(0);
	
	std::string userInput;
	while(userInput != "quit") {
		
		std::cin >> userInput;
		
		if(userInput == "write") {
			
			MakeReading(currentID, dataList, "worm");
			dataList.SaveRawData();
		
		}
		else if(userInput == "read") {
			//dataReader.ReadDataFile("11_worm.dat");
			//DOES NOT DISPLAY AND REQUIRES POINTER BRO
		}
		else if(userInput == "build") {
			DataAnalyzer dataAnalyzer;
			dataAnalyzer.BuildData();
		}
		else if(userInput == "compare") {
			MeasurementData temp = MakeReading(currentID, dataList, "worm");
			DataComparison comparitor;
			int s;
			comparitor.CompareData(temp, temp.subject, s);
			
			std::cout << "The reading has a similarity reading of: " << s << std::endl;
			
		}
		
	}
	
	getStatus(&statusFlags, &framesInMemory);
	DeviceInfo_t * devices = getDevicesInfo();
	
	std::cout << "There are: " << getDeviceCount() << " spectrometers connected!" << std::endl;
	std::cout << "Status: " << (int)statusFlags << "." << std::endl;
	std::cout << "Serial number: " << devices->R << std::endl;
	
	//Reset parameters
	resetDevice();
	
	
	//dataList.DisplayLines();
	
	//dataList.SaveRawData();
	
	FreeLibrary(hinst);
	return 0;
}

void SaveID(unsigned int id) {
	std::ofstream outFile;
	outFile.open("settings.dat");
	outFile << id;
	outFile.close();
}


//Create new reading entry
MeasurementData MakeReading(unsigned int &id, DataList &dataList, std::string subject) {
	
	if(getDeviceCount() == 0) {
		std::cout << "There are no spectrometers plugged in. Please plug one in and try again." << std::endl;
	}
	else {
		
		resetDevice();
		
		//clearMemory();
		
		//Trigger acquisition
		
		triggerAcquisition();
		
		Sleep(100);
		
		
		
		uint16_t numOfStartElement, numOfEndElement, numOfPixelsInFrame;
		uint8_t reductionMode;
		
		getFrameFormat(&numOfStartElement, &numOfEndElement, &reductionMode, &numOfPixelsInFrame);
		
		
		uint16_t * framePixelsBuffer = (uint16_t*)calloc(numOfPixelsInFrame, sizeof(uint16_t));
		getFrame(framePixelsBuffer, 0);
		
		
		//std::cout << "FRAME: " << numOfStartElement << ", " << numOfEndElement << ", " << numOfPixelsInFrame << std::endl;
		
		//Print out pixels
		for(int i = 0; i < numOfPixelsInFrame; i++) {
			//std::cout << "P: " << framePixelsBuffer[i] << std::endl;
		}
		
		
		getStatus(&statusFlags, &framesInMemory);
		std::cout << "Frames: " << framesInMemory << std::endl;
		
		uint8_t * allBytes = (uint8_t*)calloc(MAX_BYTES, sizeof(uint8_t));
		//int result = triggerAcquisition();
		//ReadData(allBytes);
		
		//ReadData(allBytes);
		//dataList.ReadInBytes(allBytes);
		
		dataList.ReadInPixels(framePixelsBuffer, numOfPixelsInFrame);
		
		dataList.SaveRawData();
		
		//Get user input
		float weight;
		std::string subtype;
		
		std::cout << "Enter the age (young/mature): ";
		std::cin >> subtype;
		std::cout << "From a scale of 1-100, how healthy is the plant? ";
		std::cin >> weight;
		weight /= 100.0;
		
		MeasurementData mData = (dataList.FormMeasurement(id, weight, subject, subtype));
		SaveMeasurement(mData, true);
		
		id++;
		SaveID(id);
		
		return mData;
	}
	
}

void SaveMeasurement(MeasurementData &mData, bool frameMode) {
	std::string fileName = std::to_string(mData.id);
	fileName += "_" + mData.subject + ".dat";
	std::ofstream outFile;
	outFile.open("Data/"+fileName, std::ios::out | std::ios::binary);
	
	
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
	std::cout << fileSize << " bytes compared to " << i*2 << std::endl;
	
	outFile.write((char*)bytes, fileSize);
	delete [] bytes;
	
	outFile.close();
}

unsigned int ReadID() {
	std::ifstream inFile;
	inFile.open("settings.dat");
	if(!inFile) {
		std::ofstream outFile;
		outFile.open("settings.dat");
		outFile << 1;
		outFile.close();
		inFile.open("settings.dat");
	}
	unsigned int output;
	inFile >> output;
	inFile.close();
	return output;
}

void ReadData(uint8_t * allBytes) {
	
	uint8_t * bytes = (uint8_t*)calloc(BYTES_TO_READ, sizeof(uint8_t));
	
	for(int i = 0; i < MAX_BYTES; i+= BYTES_TO_READ) {
		readFlash(bytes, i, BYTES_TO_READ);
		for(int j = 0; j < BYTES_TO_READ; j++) {
			if(j+i < MAX_BYTES) {
				allBytes[j+i] = bytes[j];
			}
		}
	}
	
}

bool LoadDLL() {
	
	//_clearMemory clearMemory;
	hinst = LoadLibrary("spectrlib_shared_64bits.dll");
	if(!hinst) {
		return false;
	}
	//HelloWorld = (HW)GetProcAddress(hinst, "HelloWorld");
	clearMemory = (_clearMemory)GetProcAddress(hinst, "clearMemory");
	getDeviceCount = (_getDeviceCount)GetProcAddress(hinst, "getDeviceCount");
	getStatus = (_getStatus)GetProcAddress(hinst, "getStatus");
	connectToDeviceByIndex = (_connectToDeviceByIndex)GetProcAddress(hinst, "connectToDeviceByIndex");
	getDevicesInfo = (_getDevicesInfo)GetProcAddress(hinst, "getDevicesInfo");
	resetDevice = (_resetDevice)GetProcAddress(hinst, "resetDevice");
	triggerAcquisition = (_triggerAcquisition)GetProcAddress(hinst, "triggerAcquisition");
	readFlash = (_readFlash)GetProcAddress(hinst, "readFlash");
	eraseFlash = (_eraseFlash)GetProcAddress(hinst, "eraseFlash");
	getFrameFormat = (_getFrameFormat)GetProcAddress(hinst, "getFrameFormat");
	getFrame = (_getFrame)GetProcAddress(hinst, "getFrame");
	
	return true;
}
