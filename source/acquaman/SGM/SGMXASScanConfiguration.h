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

	Q_PROPERTY(double exitSlitGap READ exitSlitGap WRITE setExitSlitGap)
	Q_PROPERTY(int grating READ grating WRITE setGrating)
	Q_PROPERTY(int harmonic READ harmonic WRITE setHarmonic)
	//NEED Q_PROPERTY for trackingGroup
	//NEED Q_PROPERTY for cfgXASDetectors_

public:
	Q_INVOKABLE explicit SGMXASScanConfiguration(QObject *parent=0);

	AMControlSet *fluxResolutionSet() const { return fluxResolutionSet_;}
	AMControlSet *trackingSet() const { return trackingSet_;}

	/// Returns an AMDetectorSet that consists of the detectors a user can choose (or choose not) to use. In this case TEY, TFY, and SDD
	AMDetectorSet* detectorChoices() const { return xasDetectors_; }
	/// Returns an AMDetectorSet that consists of all the detectors this scan can/will use (adds detectors that are always collected to the detectorChoices(), such as I0 and energy feedback)
	AMDetectorSet* allDetectors() const { return allDetectors_; }
	/// Returns the current configuration requested for the user selectable detectors
	AMDetectorInfoSet detectorChoiceConfigurations() const { return xasDetectorsCfg_; }
	/// Returns the current configuration requested for all of the detectors
	AMDetectorInfoSet allDetectorConfigurations() const;

	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController();

public slots:
	virtual bool addRegion(int index, double start, double delta, double end) { return regions_->addRegion(index, start, delta, end);}

	bool setExitSlitGap(double exitSlitGap);
	bool setGrating(SGMBeamline::sgmGrating grating);
	bool setGrating(int grating);
	bool setHarmonic(SGMBeamline::sgmHarmonic harmonic);
	bool setHarmonic(int harmonic);

	bool setTrackingGroup(AMControlInfoList trackingList);
	bool setFluxResolutionGroup(AMControlInfoList fluxResolutionList);
	/* NTBA March 14, 2011 David Chevrier
	   Need something similar for detector set
	*/

	bool setDetectorConfigurations(const AMDetectorInfoSet& xasDetectorsCfg);

signals:
	void exitSlitGapChanged(double exitSlitGap);
	void gratingChanged(int grating);
	void harmonicChanged(int harmonic);
	void trackingGroupChanged(AMControlInfoList);
	void fluxResolutionGroupChanged(AMControlInfoList);
	/* NTBA March 14, 2011 David Chevrier
	   Need something similar for detector set
	*/

protected:
	AMControlSet *fluxResolutionSet_;
	AMControlSet *trackingSet_;

	AMDetectorSet *xasDetectors_;
	AMDetectorSet *allDetectors_;
	AMDetectorInfoSet xasDetectorsCfg_;
};

#endif // ACQMAN_SGMXASSCANCONFIGURATION_H
