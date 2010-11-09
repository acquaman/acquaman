#ifndef AMXESSCAN_H
#define AMXESSCAN_H

#include "dataman/AMScan.h"

class AMXESScan : public AMScan
{
	Q_OBJECT
	Q_CLASSINFO("AMDbObject_Attributes", "shareTableWithClass=AMScan;description=XRay Absorption Scan")

public:
	explicit AMXESScan(QObject *parent = 0);

signals:

public slots:

};

#endif // AMXESSCAN_H
