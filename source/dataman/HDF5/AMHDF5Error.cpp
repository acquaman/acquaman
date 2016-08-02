#include "AMHDF5Error.h"

#include <stdio.h>
#include <wchar.h>

/* This routine prints the information for an error on the error stack
 * Note that it uses the following H5E_error_t struct, which is updated from
 * the current H5E_error_t:
 *
 * typedef struct H5E_error_t {
 *    hid_t 	  cls_id;	   Error class ID
 *    hid_t 	  maj_id;	   Major error ID
 *    hid_t       min_id;	   Minor error ID
 *    const char *func_name;	Function in which error occurred
 *    const char *file_name;	File in which error occurred
 *    unsigned	  line;		 Line in file where error occurs
 *    const char *desc;		 Optional supplied description
 * } H5E_error_t;
 *
 */

#include <QDebug>

#define AMHDF5ERROR_ERROR_STACK_NOT_AQUIRED 700000
#define AMHDF5ERROR_PRINT_FAILED 700001
#define AMHDF5ERROR_CLEAR_FAILED 700002

AMHDF5Error::AMHDF5Error(QObject *parent) : QObject(parent)
{
	hid_t errorStack = H5Eget_current_stack();

	H5Eset_auto2(NULL, NULL, NULL);
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

bool AMHDF5Error::dumpErrorStack()
{
	/// Two possible solutions to capturing the H5E error stack.
	/// Currently they require memory handling and stream parsing.

	/* Solution 1 */
	hid_t errorStack = H5Eget_current_stack();

	char* buffer;
	size_t length = 500;

	FILE *testFileStream = open_memstream(&buffer, &length);

	H5Eprint2(errorStack, testFileStream);

	fflush(testFileStream);

	//	QString testResponse = QString("MORR DID IT?\0");
	//	qDebug() << "The test response is: " << testResponse;
	//	testResponse.toWCharArray(buffer);

//	QString bufferTransfer;

	qDebug() << QString(buffer);

	/* Solution 2 */
	//	errorDump_ = new char[500];
	//	errorDump_ = "default";
	//	H5Ewalk2(H5E_DEFAULT, H5E_WALK_UPWARD, captureErrorOutput, errorDump_);
	//	qDebug() << "LOOK AT THIS" << QString(errorDump_);
}

herr_t captureErrorOutput(unsigned int n, const H5E_error2_t *err_desc, void *client_data)
{
//	client_data = malloc(sizeof(char) * 500);
//	char *majorError;
//	char *minorError;
//	char className[32];
//	char test[32] = "THIS IS A TEST";
//	size_t size = 32;

//	H5Eget_class_name(err_desc->cls_id, className, size);
//	majorError = H5Eget_major(err_desc->maj_num);
//	minorError = H5Eget_minor(err_desc->min_num);

//	memcpy(client_data, test, 32 * sizeof(char));
//	memcpy(client_data, className, 32 * sizeof(char));
//	memcpy(client_data, majorError, 32 * sizeof(char));
//	memcpy(client_data + 32, minorError, 32 * sizeof(char));

	return 0;
}
