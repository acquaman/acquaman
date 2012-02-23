#ifndef AM2DSCAN_H
#define AM2DSCAN_H

#include "dataman/AMScan.h"

/// This is a practical sublcass of AMScan which provides the details to represent any two-dimensional scan.
class AM2DScan : public AMScan
{
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "shareTableWithClass=AMScan;description=2D Scan")
public:
	/// Create a new 2D scan.  Sets the adds two scan axes.
	Q_INVOKABLE explicit AM2DScan(QObject *parent = 0);
};

#endif // AM2DSCAN_H
