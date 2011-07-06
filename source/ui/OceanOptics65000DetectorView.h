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


#ifndef OCEANOPTICS65000DETECTORVIEW_H
#define OCEANOPTICS65000DETECTORVIEW_H

#include "AMDetectorView.h"
#include "beamline/OceanOptics65000Detector.h"

class OceanOptics65000BriefDetectorView : public AMBriefDetectorView
{
Q_OBJECT
public:
	/// General invokable constructor
	Q_INVOKABLE explicit OceanOptics65000BriefDetectorView(OceanOptics65000Detector *detector = 0, bool configureOnly = false, QWidget *parent = 0);

	/// Returns pointer to the detector object
	AMDetector* detector();

protected:
	/// Control editor to display the current reading
	AMControlEditor *readingCE_;
	QHBoxLayout *hl_;
	/// Pointer to the detector
	OceanOptics65000Detector *detector_;

	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
	bool setDetector(AMDetector *detector, bool configureOnly = false);
};

class OceanOptics65000DetailedDetectorView : public AMDetailedDetectorView
{
Q_OBJECT
public:
	/// General invokable constructor
	Q_INVOKABLE explicit OceanOptics65000DetailedDetectorView(OceanOptics65000Detector *detector = 0, bool configureOnly = false, QWidget *parent = 0);

	/// Returns pointer to the detector object
	AMDetector *detector();

	/// The view is managing this created object, hook up to destroyed() if you need long-term notification
	AMDetectorInfo* configurationSettings() const;

protected slots:
	/// Handles when the control setpoint is requested
	void onControlSetpointRequested();

protected:
	/// Copies the settings from the detector
	void setConfigurationSettingsFromDetector();

protected:
	/// Pointer to the detector
	OceanOptics65000Detector *detector_;
	/// Pointer to the detector settings
	OceanOptics65000DetectorInfo *configurationSettings_;
	QGridLayout *gl_;
	/// Control editor to display the current reading
	AMControlEditor *readingCE_;
	/// Control editor to read/set the integration time
	AMControlEditor *integrationTimeCE_;

	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
	bool setDetector(AMDetector *detector, bool configureOnly = false);
};

#endif // OCEANOPTICS65000DETECTORVIEW_H
