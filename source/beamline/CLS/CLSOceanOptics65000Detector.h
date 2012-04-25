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


#ifndef CLSOCEANOPTICS65000DETECTOR_H
#define CLSOCEANOPTICS65000DETECTOR_H

#include "beamline/AMDetector.h"
#include "dataman/info/CLSOceanOptics65000DetectorInfo.h"
#include "beamline/AMControlSet.h"
#include "beamline/AMPVControl.h"

class CLSOceanOptics65000Detector : public CLSOceanOptics65000DetectorInfo, public AMDetector
{
Q_OBJECT
public:
	/// Default constructor takes a base name for generation
	CLSOceanOptics65000Detector(const QString &name, const QString &baseName, AMDetector::ReadMethod readMethod = AMDetector::ImmediateRead, QObject *parent = 0);
	/// Destructor
	~CLSOceanOptics65000Detector();

	/// Returns the meta object
	const QMetaObject* getMetaObject();

	virtual QString dacqName() const;

	/// General reading value (in this case, the total counts in all bins)
	virtual double reading() const;

	/// NEEDS TO RETURN A NEW INSTANCE, CALLER IS RESPONSIBLE FOR MEMORY.
	/// Converts to a new instance of general detector info (snapshot of current state)
	AMDetectorInfo* toInfo() const;
	/// Converts to a by value copy of specific detector info type (snapshot of current state)
	CLSOceanOptics65000DetectorInfo toOceanOptics65000Info() const;

	/// Sets current state from a pointer to specific detector info type
	bool setFromInfo(const AMDetectorInfo *info);
	/// Sets current state from a reference to specific detector info type
	bool setFromInfo(const CLSOceanOptics65000DetectorInfo &info);

	/// Returns pointer to the data waveform control
	AMControl* dataWaveformControl() const;
	/// Returns pointer to the integration time contorl
	AMControl* integrationTimeControl() const;

	/// Checks the settings passed in to confirm they match the current values within tolerance
	bool settingsMatchFbk(CLSOceanOptics65000DetectorInfo *settings);

	/// Returns the description
	QString description() const;

public slots:
	/// Sets the description for the detector
	void setDescription(const QString &description);
	/// Sets the detector contorls to the state passed in
	bool setControls(CLSOceanOptics65000DetectorInfo *settings);

protected slots:
	/// Handles when controls change their connected state
	void onControlsConnected(bool connected);
	/// Handles when the values for the reading controls change
	void onReadingsControlValuesChanged();
	/// Handles when the values for the settings controls change
	void onSettingsControlValuesChanged();

protected:
	/// The actual control for the spectrum waveform
	AMControl *dataWaveformControl_;
	/// The control for the integration time
	AMControl *integrationTimeControl_;

	/// A control set for all the controls (for ease of signalling)
	AMControlSet *allControls_;
};

#endif // CLSOCEANOPTICS65000DETECTOR_H
