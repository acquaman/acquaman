/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef MCPDETECTORINFO_H
#define MCPDETECTORINFO_H

#include "AMDetectorInfo.h"

class MCPDetectorInfo : public AMDetectorInfo
{
Q_OBJECT
	Q_PROPERTY(double hvSetpoint READ hvSetpoint WRITE setHVSetpoint)
	Q_PROPERTY(double hvSetpointRangeMin READ hvSetpointRangeMin WRITE setHVSetpointRangeMin)
	Q_PROPERTY(double hvSetpointRangeMax READ hvSetpointRangeMax WRITE setHVSetpointRangeMax)

	Q_CLASSINFO("AMDbObject_Attributes", "description=MCP Detector")

public:
	MCPDetectorInfo(const QString& name, const QString& description, QObject *parent = 0);

	MCPDetectorInfo(const MCPDetectorInfo &original);

	/// Creates a new info pointer from this one, caller is responsible for memory
	virtual AMDetectorInfo* toNewInfo() const;

	MCPDetectorInfo& operator=(const MCPDetectorInfo& other);

	/// Operational setpoint for High Voltage (HV)
	double hvSetpoint() const;
	double hvSetpointRangeMin() const;
	double hvSetpointRangeMax() const;
	QPair<double, double> hvSetpointRange() const;

	QDebug qDebugPrint(QDebug &d) const;

	// Dimensionality and size:
	////////////////////////////////////

	// Since this is a single-point detector, we're using the default rank(), size(), and axes() from AMDetectorInfo.

	virtual bool hasDetails() const;



public slots:
	void setHVSetpoint(double hvSetpoint);
	void setHVSetpointRangeMin(double min);
	void setHVSetpointRangeMax(double max);
	void setHVSetpointRange(QPair<double, double> range);

protected:
	double hvSetpointRangeMin_, hvSetpointRangeMax_, hvSetpoint_;
};


#endif // MCPDETECTORINFO_H
