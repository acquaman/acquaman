#ifndef AMXRFSCAN_H
#define AMXRFSCAN_H

#include "dataman/AMScan.h"

class AMXRFScan : public AMScan
{
	/*!
		 This is a practical subclass of AMScan which provides the details to represent an fluorescence scan.
	It creates a zero dimensional scan axis because there aren't any independent variables that are varied in the scan
	space.  All parameters are set and then data collects while you wait for it to be done.  This is why the contructor
	is left empty.
	*/
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "shareTableWithClass=AMScan;description=XRay Fluorescence Scan")

public:
	/// Constructor.  Creates a new XRF scan.
	Q_INVOKABLE explicit AMXRFScan(QObject *parent = 0);

	/// Re-implemented from AMScan.
	virtual bool loadDataImplementation();
};

#endif // AMXRFSCAN_H
