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


#ifndef SGMMCPDETECTORINFO_H
#define SGMMCPDETECTORINFO_H

#include "dataman/info/AMOldDetectorInfo.h"
#include "util/AMRange.h"

class SGMMCPDetectorInfo : public AMOldDetectorInfo
{
Q_OBJECT
	Q_PROPERTY(double hvSetpoint READ hvSetpoint WRITE setHVSetpoint)
	Q_PROPERTY(double hvSetpointRangeMin READ hvSetpointRangeMin WRITE setHVSetpointRangeMin)
	Q_PROPERTY(double hvSetpointRangeMax READ hvSetpointRangeMax WRITE setHVSetpointRangeMax)

	Q_CLASSINFO("AMDbObject_Attributes", "description=MCP Detector")

public:
 	virtual ~SGMMCPDetectorInfo();
	Q_INVOKABLE SGMMCPDetectorInfo(const QString& name = "tfy", const QString& description = "TFY", QObject *parent = 0);

	SGMMCPDetectorInfo(const SGMMCPDetectorInfo &original);

	/// Creates a new info pointer from this one, caller is responsible for memory
	virtual AMOldDetectorInfo* toNewInfo() const;

	SGMMCPDetectorInfo& operator=(const SGMMCPDetectorInfo& other);

	/// Operational setpoint for High Voltage (HV)
	double hvSetpoint() const;
	double hvSetpointRangeMin() const;
	double hvSetpointRangeMax() const;
	AMRange hvSetpointRange() const;

	QDebug qDebugPrint(QDebug &d) const;

	// Dimensionality and size:
	////////////////////////////////////

	// Since this is a single-point detector, we're using the default rank(), size(), and axes() from AMDetectorInfo.

	virtual bool hasDetails() const;



public slots:
	void setHVSetpoint(double hvSetpoint);
	void setHVSetpointRangeMin(double min);
	void setHVSetpointRangeMax(double max);
	void setHVSetpointRange(const AMRange &range);

protected:
	double hvSetpointRangeMin_, hvSetpointRangeMax_, hvSetpoint_;
};


#endif // SGMMCPDETECTORINFO_H
