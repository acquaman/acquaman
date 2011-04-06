#ifndef AMXRFSCAN_H
#define AMXRFSCAN_H

#include "dataman/AMScan.h"

class AMXRFScan : public AMScan
{
	/// This is a practical subclass of AMScan which provides the details to represent an fluorescence scan.
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "shareTableWithClass=AMScan;description=XRay Fluorescence Scan")

public:
	/// Constructor.  Creates a new XRF scan.
	Q_INVOKABLE explicit AMXRFScan(QObject *parent = 0);

	/// Re-implemented from AMScan.
	virtual bool loadDataImplementation();
};

#endif // AMXRFSCAN_H
