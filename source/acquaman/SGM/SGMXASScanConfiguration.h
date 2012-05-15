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
	Q_PROPERTY(bool undulatorTracking READ undulatorTracking WRITE setUndulatorTracking)
	Q_PROPERTY(bool monoTracking READ monoTracking WRITE setMonoTracking)
	Q_PROPERTY(bool exitSlitTracking READ exitSlitTracking WRITE setExitSlitTracking)
	Q_PROPERTY(AMDbObject* detectorConfigs READ dbReadDetectorConfigs WRITE dbLoadDetectorConfigs)

	Q_CLASSINFO("AMDbObject_Attributes", "description=SGM XAS Scan Configuration")


public:
	Q_INVOKABLE explicit SGMXASScanConfiguration(QObject *parent=0);
	SGMXASScanConfiguration(const SGMXASScanConfiguration &original);

	const QMetaObject* getMetaObject();

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

	/// Returns whether or not this scan configuration can convert a property from an enum into a useful string. For use with the export systems. SGMXASScanConfiguration can convert grating and harmonic.
	virtual bool canEnumConvert(const QString &enumName) const;

	/// Returns the useful string version for enum convertable properites. Will return [??] if the property cannot be converted by this scan configuration. SGMXASScanConfiguration can convert grating and harmonic.
	virtual QString enumConvert(const QString &enumName, int enumValue) const;

	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController();

	/// Returns a pointer to a newly-created AMScanConfigurationView that is appropriate for viewing and editing this kind of scan configuration. Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanConfigurationView* createView();

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by AMBeamlineScanAction to set the main text in the action view.
	virtual QString detailedDescription() const;

	/// Returns the AMControlInfo for the scanned region control.
	AMControlInfo regionControlInfo() const { return regions_->defaultControl()->toInfo(); }
	/// Returns the AMControlInfo for the time control.
	AMControlInfo timeControlInfo() const { return regions_->defaultTimeControl()->toInfo(); }

public slots:
	bool setTrackingGroup(AMControlInfoList trackingList);
	bool setFluxResolutionGroup(AMControlInfoList fluxResolutionList);

	bool setUndulatorTracking(bool undulatorTracking);
	bool setMonoTracking(bool monoTracking);
	bool setExitSlitTracking(bool exitSlitTracking);

	bool setExitSlitGap(double exitSlitGap);
	bool setGrating(SGMBeamline::sgmGrating grating);
	bool setGrating(int grating);
	bool setHarmonic(SGMBeamline::sgmHarmonic harmonic);
	bool setHarmonic(int harmonic);

	/* NTBA March 14, 2011 David Chevrier
	   Need something similar for detector set
	*/

	bool setDetectorConfigurations(const AMDetectorInfoSet& xasDetectorsCfg);

signals:
	void trackingGroupChanged(AMControlInfoList);
	void fluxResolutionGroupChanged(AMControlInfoList);

	void undulatorTrackingChanged(bool undulatorTracking);
	void monoTrackingChanged(bool monoTracking);
	void exitSlitTrackingChanged(bool exitSlitTracking);

	void exitSlitGapChanged(double exitSlitGap);
	void gratingChanged(int grating);
	void harmonicChanged(int harmonic);
	/* NTBA March 14, 2011 David Chevrier
	   Need something similar for detector set
	*/

protected slots:
	void detectorAvailabilityChanged(AMDetector *detector, bool isAvailable);

protected:
	AMDbObject* dbReadDetectorConfigs() { return &xasDetectorsCfg_;}
	void dbLoadDetectorConfigs(AMDbObject*) {} //Never called, xasDetectorsCfg_ is always valid

protected:
	AMControlSet *fluxResolutionSet_;
	AMControlSet *trackingSet_;

	AMDetectorSet *xasDetectors_;
	AMDetectorSet *allDetectors_;
	AMDetectorInfoSet xasDetectorsCfg_;
};

#endif // ACQMAN_SGMXASSCANCONFIGURATION_H
