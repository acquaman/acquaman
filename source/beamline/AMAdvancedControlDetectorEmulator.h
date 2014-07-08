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


#ifndef AMADVANCEDCONTROLDETECTOREMULATOR_H
#define AMADVANCEDCONTROLDETECTOREMULATOR_H

#include "beamline/AMBasicControlDetectorEmulator.h"

#include "beamline/AMBeamline.h"

class AMAdvancedControlDetectorEmulator : public AMBasicControlDetectorEmulator
{
Q_OBJECT
public:
	/// Constructor takes a name and description as well as a pointer to the control you wish to acquire
	AMAdvancedControlDetectorEmulator(const QString &name, const QString &description, AMControl *control, AMControl *statusControl, double statusAcquiringValue, double statusNotAcquiringValue, AMDetectorDefinitions::ReadMethod readMethod, const QString &synchronizedDwellKey, QObject *parent = 0);

	/// The cannot be configured in this manner
	virtual bool supportsSynchronizedDwell() const { return true; }
	/// Returns empty string
	virtual QString synchronizedDwellKey() const { return synchronizedDwellKey_;}

	/// No triggering source
	virtual bool sharesDetectorTriggerSource() { return true; }
	/// Returns Null pointer
	virtual AMDetectorTriggerSource* detectorTriggerSource() { return AMBeamline::bl()->synchronizedDwellTime()->triggerSource(); }

public slots:
	void setSynchronizedDwellKey(const QString &synchronizedDwellKey) { synchronizedDwellKey_ = synchronizedDwellKey; }

protected:
	QString synchronizedDwellKey_;
};

#endif // AMADVANCEDCONTROLDETECTOREMULATOR_H
