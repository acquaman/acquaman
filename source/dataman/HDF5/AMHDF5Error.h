#ifndef AMHDF5ERROR_H
#define AMHDF5ERROR_H

#include <QObject>

#include <hdf5.h>

#include "util/AMErrorMonitor.h"

/// This is  a c function to walk through a HDF5 error stack.
/// A work in progress.
extern "C" herr_t captureErrorOutput(unsigned int n, const H5E_error_t *err_desc, void *client_data);

/// A class to abstract the HDF5 error functions. Currently due to the structure of
/// H5E output is difficult to capture for AMErrorMonitor and options are limited.
class AMHDF5Error : public QObject
{
	Q_OBJECT

public:
	/// Constructor
	explicit AMHDF5Error(QObject *parent = 0);
	/// Destructor
	virtual ~AMHDF5Error();


signals:

public slots:
	/// Prints the stack to stdio for now.
	bool printErrorStack();
	/// Clears the current error stack.
	bool clearErrorStack();

private:

//	char *errorDump_;

};

#endif // AMHDF5ERROR_H
