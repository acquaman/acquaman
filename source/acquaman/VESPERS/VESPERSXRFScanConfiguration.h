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


#ifndef VESPERSXRFSCANCONFIGURATION_H
#define VESPERSXRFSCANCONFIGURATION_H

#include "acquaman/AMScanConfiguration.h"
#include "dataman/VESPERS/XRFDetectorInfo.h"

class VESPERSXRFScanConfiguration : public AMScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(AMDbObject* xrfDetectorInfo READ dbReadXRFDetectorInfo WRITE dbLoadXRFDetectorInfo)

	Q_CLASSINFO("AMDbObject_Attributes", "description=VESPERS XRF Scan Configuration")

public:
	/// Default constructor.
	Q_INVOKABLE explicit VESPERSXRFScanConfiguration(QObject *parent = 0);
	/// Convenience constructor.
	VESPERSXRFScanConfiguration(XRFDetectorInfo detectorInfo, QObject *parent = 0);
	/// Destructor.
	~VESPERSXRFScanConfiguration();

	/// Returns the detector info for the current detector.
	XRFDetectorInfo detectorInfo() const { return xrfDetectorInfo_; }

	/// Returns a new instance of the scan configuration.
	virtual AMScanConfiguration *createCopy() const;

	/// Returns a new instance of the scan controller.
	virtual AMScanController *createController();

	/// A human-readable description of this scan configuration. Can be re-implemented to provide more details.
	virtual QString description() const {
		return QString("XRF Free Run Scan");
	}

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const;

	/// Returns the integration time.
	double integrationTime() const { return integrationTime_; }
	/// Returns the maximum energy in eV.
	double maximumEnergy() const { return maxEnergy_; }
	/// Returns the peaking time.
	double peakingTime() const { return peakingTime_; }

public slots:
	/// Sets the detector info to the given detector info.
	void setDetectorInfo(XRFDetectorInfo info) { xrfDetectorInfo_ = info; setIntegrationTime(info.integrationTime()); setMaximumEnergy(info.maximumEnergy()); setPeakingTime(info.peakingTime()); }
	/// Sets the integration time.
	void setIntegrationTime(double time) { integrationTime_ = time; }
	/// Sets the maximum energy.
	void setMaximumEnergy(double energy) { maxEnergy_ = energy; }
	/// Sets the peaking time.
	void setPeakingTime(double time) { peakingTime_ = time; }

protected:
	/// Returns an AMDbObject pointer to the detector info.
	AMDbObject *dbReadXRFDetectorInfo() { return &xrfDetectorInfo_; }
	/// Empty function since it will never be called.
	void dbLoadXRFDetectorInfo(AMDbObject *) {}

	// Member variables.
	/// Detector info member variable.
	XRFDetectorInfo xrfDetectorInfo_;

	/// The integration time.
	double integrationTime_;
	/// The maximum energy.  Stored in eV.
	double maxEnergy_;
	/// The peaking time.
	double peakingTime_;
};

#endif // VESPERSXRFSCANCONFIGURATION_H
