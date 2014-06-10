/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef CLSPGTDETECTORINFO_H
#define CLSPGTDETECTORINFO_H

#include "AMSpectralOutputDetectorInfo.h"
#include "util/AMRange.h"

class CLSPGTDetectorInfo : public AMSpectralOutputDetectorInfo
{
Q_OBJECT
	Q_PROPERTY(double hvSetpoint READ hvSetpoint WRITE setHVSetpoint)
	Q_PROPERTY(double hvSetpointRangeMin READ hvSetpointRangeMin WRITE setHVSetpointRangeMin)
	Q_PROPERTY(double hvSetpointRangeMax READ hvSetpointRangeMax WRITE setHVSetpointRangeMax)

	Q_CLASSINFO("AMDbObject_Attributes", "description=PGT Detector")

public:
 	virtual ~CLSPGTDetectorInfo();
	Q_INVOKABLE CLSPGTDetectorInfo(const QString& name = "pgt", const QString& description = "SDD", QObject *parent = 0);

	CLSPGTDetectorInfo(const CLSPGTDetectorInfo &original);

	/// Creates a new info pointer from this one, caller is responsible for memory
	virtual AMOldDetectorInfo* toNewInfo() const;

	CLSPGTDetectorInfo& operator=(const CLSPGTDetectorInfo& other);

	/// Operational setpoint for High Voltage (HV)
	double hvSetpoint() const;
	double hvSetpointRangeMin() const;
	double hvSetpointRangeMax() const;
	AMRange hvSetpointRange() const;

	QDebug qDebugPrint(QDebug &d) const;

	// Dimensionality and size:
	////////////////////////////////////

	// Using the base class (AMSpectralOutputDetectorInfo) for default rank(), size(), and axes().


	virtual bool hasDetails() const;


public slots:
	void setHVSetpoint(double hvSetpoint);
	void setHVSetpointRangeMin(double min);
	void setHVSetpointRangeMax(double max);
	void setHVSetpointRange(const AMRange &range);

protected:
	double hvSetpointRangeMin_, hvSetpointRangeMax_, hvSetpoint_;

};


#endif // CLSPGTDETECTORINFO_H
