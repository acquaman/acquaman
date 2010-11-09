#ifndef AMXESSCAN_H
#define AMXESSCAN_H

#include "dataman/AMScan.h"
#include "acquaman.h"

class AMXESScan : public AMScan
{
	Q_OBJECT
	Q_CLASSINFO("AMDbObject_Attributes", "shareTableWithClass=AMScan;description=XRay Absorption Scan")

	Q_PROPERTY(AMIntList intList READ intList WRITE setIntList)

	Q_PROPERTY(AMDoubleList doubleList READ doubleList WRITE setDoubleList)
	Q_PROPERTY(AMDbObjectList objectList READ objectList WRITE setObjectList)


public:
	explicit AMXESScan(QObject *parent = 0);

	AMIntList intList() const { return il_; }
	void setIntList(const AMIntList& il) { il_ = il; }
	AMDoubleList doubleList() const { return dl_; }
	void setDoubleList(const AMDoubleList& dl) {dl_ = dl; }
	AMDbObjectList objectList() const { return ol_; }
	void setObjectList(const AMDbObjectList& ol) { ol_ = ol; }

signals:

public slots:

protected:
	AMIntList il_;


	AMDoubleList dl_;


	AMDbObjectList ol_;



};

#endif // AMXESSCAN_H
