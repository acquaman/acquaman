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


#ifndef AM_SGMXASSCANCONFIGURATION_H
#define AM_SGMXASSCANCONFIGURATION_H

#include "acquaman/AMStepScanConfiguration.h"
#include "SGMScanConfiguration.h"
#include "dataman/info/AMOldDetectorInfoSet.h"

class SGMXASScanConfiguration : public AMStepScanConfiguration, public SGMScanConfiguration
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
	/// Standard constructor, queries the SGMBeamline and retrieves some detector information
	Q_INVOKABLE explicit SGMXASScanConfiguration(QObject *parent=0);
	/// Copy constructor
	SGMXASScanConfiguration(const SGMXASScanConfiguration &original);
	virtual ~SGMXASScanConfiguration();

	/// Returns the metaObject
	const QMetaObject* getMetaObject();

	/// Returns the current configuration requested for the user selectable detectors
	AMOldDetectorInfoSet detectorChoiceConfigurations() const { return xasDetectorsCfg_; }

	/// Returns whether or not this scan configuration can convert a property from an enum into a useful string. For use with the export systems. SGMXASScanConfiguration can convert grating and harmonic.
	virtual bool canEnumConvert(const QString &enumName) const;

	/// Returns the useful string version for enum convertable properites. Will return [??] if the property cannot be converted by this scan configuration. SGMXASScanConfiguration can convert grating and harmonic.
	virtual QString enumConvert(const QString &enumName, int enumValue) const;

	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;

	/// Returns a NULL pointer as this scan configuration is no longer supported for running scans
	virtual AMScanController* createController();

	/// Returns a pointer to a newly-created AMScanConfigurationView that is appropriate for viewing and editing this kind of scan configuration. Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanConfigurationView* createView();

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const;

	/// Overrides the warnings string to check warnings from the detector info set
	virtual QString dbLoadWarnings() const;

public slots:
	/// Sets the tracking group's values from an info list
	bool setTrackingGroup(AMControlInfoList trackingList);
	/// Sets the flux/resolution group's values from an info list
	bool setFluxResolutionGroup(AMControlInfoList fluxResolutionList);

	/// Sets the undulator tracking directly
	bool setUndulatorTracking(bool undulatorTracking);
	/// Sets the mono tracking directly
	bool setMonoTracking(bool monoTracking);
	/// Sets the exit slit tracking directly
	bool setExitSlitTracking(bool exitSlitTracking);

	/// Sets the exit slit gap value for this configuration
	bool setExitSlitGap(double exitSlitGap);
	/// Sets the grating value for this configuration (as the SGMBeamline enum)
	bool setGrating(SGMBeamlineInfo::sgmGrating grating);
	/// Sets the grating value for this configuration (as an int)
	bool setGrating(int grating);
	/// Sets the harmonvic value for this configuration (as the SGMBeamline enum)
	bool setHarmonic(SGMBeamlineInfo::sgmHarmonic harmonic);
	/// Sets the harmonvic value for this configuration (as an int)
	bool setHarmonic(int harmonic);

	/// Sets the detector configurations based on a detector info set
	bool setDetectorConfigurations(const AMOldDetectorInfoSet& xasDetectorsCfg);

signals:
	/// Emitted when something about the tracking group changes (information in the control info list)
	void trackingGroupChanged(AMControlInfoList);
	/// Emitted when something about the flux/resolution group changes (information in the control info list)
	void fluxResolutionGroupChanged(AMControlInfoList);

	/// Emitted when the requested tracking for the undulator changes
	void undulatorTrackingChanged(bool undulatorTracking);
	/// Emitted when the requested tracking for the mono changes
	void monoTrackingChanged(bool monoTracking);
	/// Emitted when the requested tracking for the exit slit changes
	void exitSlitTrackingChanged(bool exitSlitTracking);

protected:
	/// Used to write the detector configurations to the database
	AMDbObject* dbReadDetectorConfigs() { return &xasDetectorsCfg_;}
	/// For database loading (never called)
	void dbLoadDetectorConfigs(AMDbObject*) {} //Never called, xasDetectorsCfg_ is always valid

protected:
	/// The configurations for the detectors
	AMOldDetectorInfoSet xasDetectorsCfg_;
};

#endif // AM_SGMXASSCANCONFIGURATION_H
