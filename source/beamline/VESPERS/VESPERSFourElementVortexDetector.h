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


#ifndef VESPERSFOURELEMENTVORTEXDETECTOR_H
#define VESPERSFOURELEMENTVORTEXDETECTOR_H

#include "beamline/AMXRFDetector.h"

/// Implentation of AMXRFDetector for the single element vortex detector used on VESPERS.
class VESPERSFourElementVortexDetector : public AMXRFDetector
{
	Q_OBJECT

public:
	/// Default constructor. Requires the name and description of the detector. It builds all the PV's and connects them accordingly.
	VESPERSFourElementVortexDetector(const QString &name, const QString &description, QObject *parent = 0);
	/// Destructor.
	virtual ~VESPERSFourElementVortexDetector();

	/// Returns a string with a human readable text of what is important about this detector.
	virtual QString details() const;
	/// The Vortex doesn't explicitly require powering on
	virtual bool requiresPower() const { return false; }

	/// Cancelling is implemented for the Vortex detectors
	virtual bool canCancel() const { return true; }
	/// Clearing is not currently supported for the Vortex detectors
	virtual bool canClear() const { return false; }

	/// Vortex detectors are not currently capable of continuous acquisition
	virtual bool canContinuousAcquire() const { return false; }

	/// The Vortex can be configured to work with synchronized dwell time systems
	virtual bool supportsSynchronizedDwell() const { return false; }
	/// Returns the CLS Synchronized Dwell Time trigger PV string, which acts as the key for the synchronized dwell time lookup system
	virtual QString synchronizedDwellKey() const;

	/// Returns RequestRead as the type
	virtual AMDetectorDefinitions::ReadMethod readMethod() const { return AMDetectorDefinitions::RequestRead; }
	/// Returns SingleRead as the type
	virtual AMDetectorDefinitions::ReadMode readMode() const { return AMDetectorDefinitions::SingleRead; }

	/// The vortex detectors support elapsed time.
	virtual bool supportsElapsedTime() const { return true; }

	/// Returns the maximum energy for this detector.  Result returned in keV.
	double maximumEnergy() const { return maximumEnergyControl_->value(); }
	/// Returns the peaking time for this detector.  Result returned in us.
	double peakingTime() const { return peakingTimeControl_->value(); }

signals:
	/// Notifier that the maximum energy has changed.  Value passed in keV.
	void maximumEnergyChanged(double);
	/// Notifier that the peaking time has changed.  Value passed in us.
	void peakingTimeChanged(double);

public slots:

	/// The read mode cannot be changed for Amptek detectors
	virtual bool setReadMode(AMDetectorDefinitions::ReadMode readMode);

	/// Vortex detectors do not support clearing
	virtual bool clear() { return false; }

	/// Sets the maximum energy.  Expects \param energy in keV.
	void setMaximumEnergy(double energy);
	/// Sets the peaking time.  Expects \param time in us.
	void setPeakingTime(double time);

protected slots:
	/// Handles changing the scale for the raw spectra sources when the maximum energy changes.
	void onMaximumEnergyChanged(double newMaximum);

protected:
	/// The maximum energy control.
	AMPVControl *maximumEnergyControl_;
	/// The peaking time control.
	AMPVControl *peakingTimeControl_;
};

#endif // VESPERSFOURELEMENTVORTEXDETECTOR_H
