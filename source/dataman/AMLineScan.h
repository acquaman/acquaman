#ifndef AMLINESCAN_H
#define AMLINESCAN_H

#include "dataman/AMScan.h"

class AMLineScan : public AMScan
{
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "shareTableWithClass=AMScan;description=Line Scan")

public:
	/// Constructor.
	Q_INVOKABLE explicit AMLineScan(QObject *parent = 0);
};

#endif // AMLINESCAN_H
