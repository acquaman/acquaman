#include "AMHDF5Error.h"

#include <stdio.h>

#define AMHDF5ERROR_HDF5_ERRROR 700000
#define AMHDF5ERROR_ERROR_STACK_NOT_AQUIRED 700001
#define AMHDF5ERROR_PRINT_FAILED 700002
#define AMHDF5ERROR_CLEAR_FAILED 700003

AMHDF5Error::AMHDF5Error(QObject *parent) : QObject(parent)
{
//	H5Eset_auto2(NULL, NULL, NULL);
}

AMHDF5Error::~AMHDF5Error()
{

}

bool AMHDF5Error::printErrorStack()
{
	hid_t errorStack = H5Eget_current_stack();

	if(errorStack < 0){
		AMErrorMon::alert(this, AMHDF5ERROR_ERROR_STACK_NOT_AQUIRED, QString("Print: HDF5 error-stack could not be aquired."));
		return false;
	}

	herr_t printStatus = H5Eprint2(errorStack, stdout);

	if(printStatus){
		AMErrorMon::alert(this, AMHDF5ERROR_PRINT_FAILED, QString("Print: HDF5 error-stack could not be printed."));
		return false;
	}

	return true;
}

bool AMHDF5Error::clearErrorStack()
{
	hid_t errorStack = H5Eget_current_stack();

	if(errorStack < 0){
		AMErrorMon::alert(this, AMHDF5ERROR_ERROR_STACK_NOT_AQUIRED, QString("Print: HDF5 error-stack could not be aquired."));
		return false;
	}

	herr_t clearStatus = H5Eclear2(errorStack);

	if(clearStatus){
		AMErrorMon::alert(this, AMHDF5ERROR_CLEAR_FAILED, QString("Print: HDF5 error-stack could not be printed."));
		return false;
	}

	return true;
}

bool AMHDF5Error::dumpErrorStack(int errorCode)
{
	hid_t errorStack = H5Eget_current_stack();

	if(errorStack < 0){
		AMErrorMon::alert(this, AMHDF5ERROR_ERROR_STACK_NOT_AQUIRED, QString("Dump: HDF5 error-stack could not be aquired."));
		return false;
	}

	char* buffer;

	size_t length = 2048;

	FILE *testFileStream = open_memstream(&buffer, &length);

	herr_t status = H5Eprint2(errorStack, testFileStream);

	if(status < 0){
		AMErrorMon::alert(this, errorCode, QString("Dummp: Printing to filestream failed, error-stack output not aquired."));
		fclose(testFileStream);
		return false;
	}

	fflush(testFileStream);

	AMErrorMon::alert(this, errorCode, QString(buffer));

	fclose(testFileStream); //Stream is allocated in open_memstream and must be closed/deallocated.

	return true;
}

bool AMHDF5Error::muteHDF5ErrorOutput()
{
	herr_t status = H5Eset_auto2(NULL, NULL, NULL);

	if(status < 0){
		//AM
		return false;
	}

	return true;
}
