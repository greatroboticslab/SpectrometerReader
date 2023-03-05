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

typedef int (*_setAcquisitionParameters) (const uint16_t, const uint16_t, const uint8_t, const uint32_t);
_setAcquisitionParameters setAcquisitionParameters;

HINSTANCE hinst;

bool LoadDLL();

void ReadData(uint8_t*);

unsigned int ReadID();

MeasurementData MakeReading(unsigned int &id, DataList &dataList, std::string subject, bool, bool, int);

DataList ReadSpectrum;

void SaveMeasurement(MeasurementData &mData, bool);

void SaveID(unsigned int);

bool DeviceCheck();

unsigned char statusFlags;
unsigned short framesInMemory;
DeviceInfo_t * device;

std::string currentSubject, currentSubtype;
float currentWavelength, currentThreshold;

bool deviceConnected = false;
bool currentHealthy = false;

bool saveBinary = true;
bool readRaw = false;

int readingLayers = 1; //Default is 10
bool applyBaseline = true;

void DisplayHelp(bool);

int main(int argc, char **argv) {
	
	unsigned int currentID = ReadID();
	
	DataList dataList;
	dataList.ReadWaveValues();
	DataReader dataReader;
	
	if(!LoadDLL()) {
		std::cout << "Could not locate the required .dll file (spectrlib_shared_64bits.dll)" << std::endl;
		return ERROR_NO_DLL;
	}
	
	//PUT STUFF AFTER LOADING DLL
	
	/*
	if(getDeviceCount() == 0) {
		std::cout << "No spectrometer detected! Try running again after pluggin in the spectrometer." << std::endl;
		return ERROR_NOT_CONNECTED;
	}
	*/
	//connectToDeviceByIndex(0);
	
	
	if(argc <= 1) {
	
		//User input terminal
		std::string userInput;
		while(userInput != "quit") {
			
			
			
			std::cin >> userInput;
			
			
			//Make a reading and save it
			if(userInput == "read") {
				
				if(DeviceCheck()) {
					MakeReading(currentID, dataList, currentSubject, true, true, readingLayers);
					dataList.SaveRawData();
				}
				
			}
			//Build reference files
			else if(userInput == "build") {
				DataAnalyzer dataAnalyzer;
				dataAnalyzer.BuildData();
			}
			//Make a reading and find the closest measurement
			else if(userInput == "compare") {
				if(DeviceCheck()) {
					MeasurementData temp = MakeReading(currentID, dataList, "temp", false, false, readingLayers);
					DataComparison comparitor;
					comparitor.CompareData(temp, currentThreshold);
				}
				
			}
			//Set wavelength
			else if(userInput == "wavelength") {
				
				std::cout << "Enter current wavelength (nm): ";
				std::cin >> currentWavelength;
				
			}
			//Set subject
			else if(userInput == "subject") {
				
				std::cout << "Enter the name of the subject: ";
				std::cin >> currentSubject;
				
			}
			//Set threshold
			else if(userInput == "threshold") {
				
				std::cout << "Set threshold of wavelength (nm): ";
				std::cin >> currentThreshold;
				
			}
			//Set iterations per reading
			else if(userInput == "iterations") {
				
				std::cout << "Set amount of iterations per reading: ";
				std::cin >> readingLayers;
				
			}
			else if(userInput == "status") {
				
				if(DeviceCheck()) {
					
					DeviceInfo_t * devices = getDevicesInfo();
		
					std::cout << "There are: " << getDeviceCount() << " spectrometers connected!" << std::endl;
					std::cout << "Status: " << (int)statusFlags << "." << std::endl;
					std::cout << "Serial number: " << devices->R << std::endl;
					
				}
				
			}
			else if(userInput == "csv") {
				
				std::string fname;
				std::cout << "Please enter the name of the file to convert to csv: ";
				std::cin >> fname;
				DataReader reader;
				MeasurementSaver saver;
				MeasurementData* m = reader.ReadDataFile("Data/" + fname);
				fname = fname.substr(0, fname.size()-4);
				saver.SaveCSV(m, fname, true, false);
				
			}
			//Debug read reference file
			else if(userInput == "bread") {
				
				MeasurementData *mData;
				int count;
				DataReader reader;
				reader.ReadReference("BuiltData/worm.dat", mData, count);
				
				std::cout << "The reference has " << count << " entries." << std::endl;
				
			}
			else {
				
				if(userInput != "quit") {
					DisplayHelp(false);
				}
				
			}
			
		}
	}
	
	//Use from command line
	
	
	else {
		bool makeReading = false;
		bool buildData = false;
		bool compareData = false;
		//std::cout << "USING COMMANDS" << std::endl;
		
		int c = 1;
		
		while(c < argc) {
			
			//std::cout << argv[c] << std::endl;
			
			if(strcmp(argv[c], "-subject") == 0) {
				
				
				
				c++;
				currentSubject = argv[c];
				
				std::cout << "CURRENT SUBJECT " << currentSubject << std::endl;
				
			}
			
			else if(strcmp(argv[c], "-wavelength") == 0) {
				
				c++;
				currentWavelength = std::stof(argv[c]);
				
			}
			else if(strcmp(argv[c], "-threshold") == 0) {
				
				c++;
				currentThreshold = std::stof(argv[c]);
				
			}
			else if(strcmp(argv[c], "-health") == 0 || strcmp(argv[c], "-healthy") == 0) {
				c++;
				if(strcmp(argv[c], "y") == 0 || strcmp(argv[c], "Y") == 0 || strcmp(argv[c], "1") == 0 || strcmp(argv[c], "yes") == 0 || strcmp(argv[c], "true") == 0) {
					
					currentHealthy = true;
					
				}
				else {
					currentHealthy = false;
				}
			}
			else if(strcmp(argv[c], "-subtype") == 0) {
				c++;
				currentSubtype = argv[c];
			}
			else if(strcmp(argv[c], "-read") == 0) {
				
				makeReading = true;
				
			}
			else if(strcmp(argv[c], "-build") == 0) {
				
				buildData = true;
				
			}
			else if(strcmp(argv[c], "-compare") == 0) {
				
				compareData = true;
				
			}
			else if(strcmp(argv[c], "-nobinary") == 0) {
				
				saveBinary = false;
				
			}
			else if(strcmp(argv[c], "-unknown") == 0) {
				readRaw = true;
				currentSubject = "unknown";
				currentHealthy = false;
				currentSubtype = "unknown";
				
			}
			
			else if(strcmp(argv[c], "-iterations") == 0) {
				
				c++;
				readingLayers = std::stoi(argv[c]);
				
			}
			
			else {
				
				DisplayHelp(true);
				return -1;
				
			}
			
			c++;
			
		}
		
		if(makeReading) {
			
			if(DeviceCheck()) {
				MakeReading(currentID, dataList, currentSubject, false, true, readingLayers);
				dataList.SaveRawData();
			}
			
		}
		if(buildData) {
			
			DataAnalyzer dataAnalyzer;
			dataAnalyzer.BuildData();
			
		}
		
		if(compareData) {
			
			if(DeviceCheck()) {
				MeasurementData temp = MakeReading(currentID, dataList, "temp", false, false, readingLayers);
				DataComparison comparitor;
				comparitor.CompareData(temp, currentThreshold);
			}
			
		}
		
	}
	
	getStatus(&statusFlags, &framesInMemory);
	
	
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
//Result is then averaged by amount parameter
MeasurementData MakeReading(unsigned int &id, DataList &dataList, std::string subject,bool askForArgs, bool save, int amount) {
	
	if(DeviceCheck()) {
		
		int numOfScans = 5;
		
		resetDevice();
		
		setAcquisitionParameters(numOfScans, 0, 3, 200);
		
		//clearMemory();
		
		//Trigger acquisition
		
		triggerAcquisition();
		
		Sleep(500);
		
		
		
		uint16_t numOfStartElement, numOfEndElement, numOfPixelsInFrame;
		uint8_t reductionMode;
		
		getFrameFormat(&numOfStartElement, &numOfEndElement, &reductionMode, &numOfPixelsInFrame);
		
		
		uint16_t * framePixelsBuffer = (uint16_t*)calloc(numOfPixelsInFrame, sizeof(uint16_t));
		getFrame(framePixelsBuffer, 0xFFFF);
		
		
		//std::cout << "FRAME: " << numOfStartElement << ", " << numOfEndElement << ", " << numOfPixelsInFrame << std::endl;
		
		//Print out pixels
		for(int i = 0; i < numOfPixelsInFrame; i++) {
			//std::cout << "P: " << framePixelsBuffer[i] << std::endl;
		}
		
		
		getStatus(&statusFlags, &framesInMemory);
		//std::cout << "Frames: " << framesInMemory << std::endl;
		
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
		
		subtype = currentSubtype;
		
		if(askForArgs) {
		
			std::cout << "Enter the age (young/mature): ";
			std::cin >> subtype;
		
		}
		//std::cout << "From a scale of 1-100, how healthy is the plant? ";
		//std::cin >> weight;
		//weight /= 100.0;
		
		weight = currentWavelength;
		
		bool healthy = currentHealthy;
		
		if(askForArgs) {
		
			std::cout << "Is this subject healthy? (y/n): ";
			std::string yn;
			std::cin >> yn;
			if(yn == "y" || yn == "Y") {
				
				healthy = true;
				
			}
		}
		
		
		MeasurementData mData;
		//Average layers
		for(int i = 0; i < amount; i++) {
			if(i == 0) {
				mData = (dataList.FormMeasurement(id, weight, subject, subtype, healthy));
			}
			else {
				resetDevice();
				
				triggerAcquisition();
		
				Sleep(10);
				
				getFrame(framePixelsBuffer, 0);
				dataList.ReadInPixels(framePixelsBuffer, numOfPixelsInFrame);
				
				MeasurementData addData = (dataList.FormMeasurement(id, weight, subject, subtype, healthy));
				
				//Add waves
				for(int w = 0; w < mData.waveCount; w++) {
					
					
					
					mData.waves[w].intensity += addData.waves[w].intensity;
				}
				
			}
			
		}
		
		//Divide intensities
		for(int w = 0; w < mData.waveCount; w++) {
			mData.waves[w].intensity /= amount;
		}
		
		//Apply baseline
		if(applyBaseline) {
			
			float y1, y2;
			
			float sum = 0;
			//Get first 16 pixels
			for(int x = 0; x < 16; x++) {
				sum += mData.waves[x].intensity;
			}
			y1 = sum/16;
			
			//Get last 14 pixels
			sum = 0;
			for(int x = 1; x < 15; x++) {
				sum += mData.waves[mData.waveCount-x].intensity;
			}
			y2 = sum/14;
			
			//Apply it
			
			float slope = (y2-y1)/(mData.waveCount);
			
			float intercept = y2;
			if(y1 < y2) {
				intercept = y1;
			}
			
			for(int x = 0; x < mData.waveCount; x++) {
				mData.waves[x].intensity -= (intercept+(slope*x));
				if(mData.waves[x].intensity < 0) {
					mData.waves[x].intensity = 0;
				}
			}
			
		}
		
		if(save) {
			MeasurementSaver measurementSaver;
			if(saveBinary) {
				measurementSaver.SaveMeasurement(mData, true);
			}
			measurementSaver.SaveCSV(&mData, "", false, readRaw);
		}
		
		id++;
		SaveID(id);
		
		return mData;
	}
	
}

bool DeviceCheck() {
	
	if(getDeviceCount() == 0) {
			deviceConnected = false;
	}
	else {
		if(!deviceConnected) {
			connectToDeviceByIndex(0);
			deviceConnected = true;
		}
	
	}
	
	if(deviceConnected) {
		return true;
	}
	std::cout << "There are no spectrometers plugged in. Please plug one in and try again." << std::endl;
	return false;
	
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
	setAcquisitionParameters = (_setAcquisitionParameters)GetProcAddress(hinst, "setAcquisitionParameters");
	
	return true;
}

//Show help screen
void DisplayHelp(bool commandLine) {
	
	if(commandLine) {
		std::cout << "Usage:" << std::endl;
		std::cout << "-subject <string>		Sets the name of the subject." << std::endl;
		std::cout << "-subtype <string>		Sets the subtype of the subject (a string)." << std::endl;
		std::cout << "-wavelength <float>		Sets the wavelength that this measurement was using." << std::endl;
		std::cout << "-healthy <true/false>		Is the subject healthy?" << std::endl;
		std::cout << "-read		Take a measurement using the set parameters, and save it to Data/<id>_<subject>.dat" << std::endl;
		std::cout << "-build		Build all of the .dat files in /Data/ into reference files in /BuiltData/" << std::endl;
		std::cout << "-threshold	Specify the threshold in nm for comparing references." << std::endl;
		std::cout << "-compare	Take a measurement and output the closest matching reference entry." << std::endl;
	}
	else {
		
		std::cout << "Commands:" << std::endl;
		std::cout << "read	Read in data from the spectrometer." << std::endl;
		std::cout << "build	Compile all saved data into reference files." << std::endl;
		std::cout << "subject	Specify the name of the subject." << std::endl;
		std::cout << "wavelength	Specify what wavelength laser you are using." << std::endl;
		std::cout << "threshold	Specify the threshold in nm for comparing references." << std::endl;
		std::cout << "compare	Take in a reading and output the closest matching reference entry." << std::endl;
		std::cout << "status	Get the status of how many spectrometers are connected, and its serial number." << std::endl;
		std::cout << "quit	Exits the program." << std::endl;
		std::cout << "help	Display a super helpful command menu (no need to thank me, just doing my job!)" << std::endl;
		
	}
	
}
