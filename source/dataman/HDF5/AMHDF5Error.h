#ifndef AMHDF5ERROR_H
#define AMHDF5ERROR_H

#include <QObject>

#include <hdf5.h>

#include "util/AMErrorMonitor.h"

#define AMHDF5ERROR_HDF5_ERRROR 700000

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
	/// Takes the current HDF5 error stack and outputs it to the AMErrorMonitor. Error stack parsing may be a future addition.
	bool dumpErrorStack(int	errorCode = AMHDF5ERROR_HDF5_ERRROR);
	/// Mutes the HDF5 default error stack output. Not muting the error stack will result in the stack being output to stderr and then cleared.
	/// To dump the stack to AMErrorMonitor, output must be muted. Do not mute multiple times, HDF5 can do weird things if you do.
	bool muteHDF5ErrorOutput();


};

#endif // AMHDF5ERROR_H
