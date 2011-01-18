/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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
