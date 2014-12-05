/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef AMAXISSTARTEDACTIONINFO_H
#define AMAXISSTARTEDACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/AMScanAxis.h"

class AMAxisStartedActionInfo : public AMActionInfo3
{
	Q_OBJECT

	Q_PROPERTY(QString axisName READ axisName WRITE setAxisName)
	Q_PROPERTY(int axisType READ dbAxisType WRITE setDbAxisType)

public:
	/// Constructor
	Q_INVOKABLE AMAxisStartedActionInfo(const QString &axisName, AMScanAxis::AxisType axisType, QObject *parent = 0);
	/// Destructor.
	virtual ~AMAxisStartedActionInfo();

	/// Copy Constructor
	AMAxisStartedActionInfo(const AMAxisStartedActionInfo &other);

	/// This function is used as a virtual copy constructor
	virtual AMActionInfo3* createCopy() const;

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "Start Axis"; }
	/// Returns the name of the axis as a string.
	QString axisName() const { return axisName_; }
	/// Returns the type of axis.
	AMScanAxis::AxisType axisType() const { return axisType_; }

protected:
	/// Getter for database loading.
	int dbAxisType() const { return (int)axisType_; }
	/// Sets the axis name.
	void setAxisName(const QString &axisName) { axisName_ = axisName; }
	/// Sets the axis type.
	void setAxisType(AMScanAxis::AxisType axisType) { axisType_ = axisType; }
	/// Setter for database saving.
	void setDbAxisType(int axisType) { axisType_ = (AMScanAxis::AxisType)axisType; }

protected:
	/// String holding the name of the axis.
	QString axisName_;
	/// The flag holder for what kind of axis this is.
	AMScanAxis::AxisType axisType_;
};

#endif // AMAXISSTARTEDACTIONINFO_H
