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


#ifndef ACQMAN_SGMXASSCANCONFIGURATION_H
#define ACQMAN_SGMXASSCANCONFIGURATION_H

#include "acquaman/AMXASScanConfiguration.h"
#include "SGMScanConfiguration.h"

class SGMXASScanConfiguration : public AMXASScanConfiguration, public SGMScanConfiguration
{
	Q_OBJECT
public:
	SGMXASScanConfiguration(QObject *parent=0);

	AMControlSet *fluxResolutionSet() const { return fluxResolutionSet_;}
	AMControlSet *trackingSet() const { return trackingSet_;}
	AMDetectorInfoSet *detectorSet() const { return XASDetectors_;}
	AMDetectorInfoSet *cfgDetectorInfoSet() const { return cfgXASDetectors_;}

	QList<AMDetectorInfo*> usingDetectors() const;

public slots:
	virtual bool addRegion(size_t index, double start, double delta, double end) { return regions_->addRegion(index, start, delta, end);}

	bool setExitSlitGap(double exitSlitGap);
	bool setGrating(SGMBeamline::sgmGrating grating);
	bool setGrating(int grating);
	bool setHarmonic(SGMBeamline::sgmHarmonic harmonic);
	bool setHarmonic(int harmonic);
	bool setUndulatorTracking(bool track);
	bool setUndulatorTracking(int track);
	bool setMonoTracking(bool track);
	bool setMonoTracking(int track);
	bool setExitSlitTracking(bool track);
	bool setExitSlitTracking(int track);
	bool setUsingTEY(bool active);
	bool setUsingTEY(int checkedState);
	bool setUsingTFY(bool active);
	bool setUsingTFY(int checkedState);
	bool setUsingPGT(bool active);
	bool setUsingPGT(int checkedState);

	bool setCfgDetectorInfoSet(AMDetectorInfoSet *cfgDetectorInfoSet) { cfgXASDetectors_ = cfgDetectorInfoSet; return true; }


signals:
	void exitSlitGapChanged(double exitSlitGap);
	void gratingChanged(int grating);
	void harmonicChanged(int harmonic);
	void undulatorTrackingChanged(int track);
	void monoTrackingChanged(int track);
	void exitSlitTrackingChanged(int track);
	void usingTEYChanged(bool active);
	void usingTFYChanged(bool active);
	void usingPGTChanged(bool active);

protected:
	AMControlSet *fluxResolutionSet_;
	AMControlSet *trackingSet_;
	AMDetectorInfoSet *feedbackDetectors_;
	AMDetectorInfoSet *XASDetectors_;
	AMDetectorInfoSet *cfgXASDetectors_;

};

#endif // ACQMAN_SGMXASSCANCONFIGURATION_H
