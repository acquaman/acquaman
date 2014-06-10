#ifndef AM3DSCAN_H
#define AM3DSCAN_H

#include "dataman/AMScan.h"

/// This is a practical subclass of AMScan which provides the details to represent a three-dimensional scan.
class AM3DScan : public AMScan
{
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "shareTableWithClass=AMScan;description=3D Scan")

public:
	/// Creates a new 3D scan.
 	virtual ~AM3DScan();
	Q_INVOKABLE explicit AM3DScan(QObject *parent = 0);
};

#endif // AM3DSCAN_H
