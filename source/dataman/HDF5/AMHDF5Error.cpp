#include "AMHDF5Error.h"

#define AMHDF5ERROR_ERROR_STACK_NOT_AQUIRED 700000
#define AMHDF5ERROR_PRINT_FAILED 700001
#define AMHDF5ERROR_CLEAR_FAILED 700002

AMHDF5Error::AMHDF5Error(QObject *parent) : QObject(parent)
{

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

/// This function may be removed down the road if a solution on error stack capturing is not found.
herr_t captureErrorOutput(unsigned int n, const H5E_error2_t *err_desc, void *client_data)
{
//	char majorError[32] = "";
//	const char minorError[32] = "";

//	H5Eget_major(err_desc->maj_num);
//	H5Eget_minor(err_desc->min_num);

//	memcpy((char *)client_data, majorError, 32 * sizeof(char));
//	memcpy(client_data + 32, minorError, 32 * sizeof(char));

	return 0;
}
