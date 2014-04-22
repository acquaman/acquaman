#ifndef SGMSSCANCONFIGURATIONDBOBJECT_H
#define SGMSSCANCONFIGURATIONDBOBJECT_H

#include "dataman/database/AMDbObject.h"
#include "dataman/SGM/SGMBeamlineInfo.h"
#include "dataman/info/AMControlInfoList.h"

#include "dataman/AMSamplePre2013.h"

/// This class is the common database object for all scan configurations for the SGM beamline.
/*!
	This class holds all of the persistent values that all scan configurations will need.  It is meant
	to consolidate all the duplications from all the scan configurations.  The only requirement for
	any class is that they'll need to set it in their Q_PROPERTY so that they properly load the
	configuration.
  */
class SGMSScanConfigurationDbObject : public AMDbObject
{
Q_OBJECT

Q_PROPERTY(AMDbObject* trackingGroup READ dbGetTrackingGroup WRITE dbLoadTrackingGroup)
Q_PROPERTY(AMDbObject* fluxResolutionGroup READ dbGetFluxResolutionGroup WRITE dbLoadFluxResolutionGroup)

Q_CLASSINFO("AMDbObject_Attributes", "description=SGM Scan Configuration Database Object")

public:
	/// Constructor
 	virtual ~SGMSScanConfigurationDbObject();
	Q_INVOKABLE SGMSScanConfigurationDbObject(QObject *parent = 0);
	/// Copy Constructor
	SGMSScanConfigurationDbObject(const SGMSScanConfigurationDbObject &original);

	/// Returns the current trackingGroup values
	const AMControlInfoList& trackingGroup() const { return trackingGroup_; }
	/// Returns the mono tracking value
	bool monoTracking() const;
	/// Returns the undulator tracking value
	bool undulatorTracking() const;
	/// Returns the exit slit position tracking value
	bool exitSlitTracking() const;

	/// Returns the current flux/resolution group values
	const AMControlInfoList& fluxResolutionGroup() const { return fluxResolutionGroup_; }
	/// Returns the exit slit gap value
	double exitSlitGap() const;
	/// Returns the grating value
	SGMBeamlineInfo::sgmGrating grating() const;
	/// Returns the harmonic value
	SGMBeamlineInfo::sgmHarmonic harmonic() const;

signals:
	/// Notifier that the trackingGroup has changed
	void trackingGroupChanged();
	/// Notifier that the mono tracking has changed
	void monoTrackingChanged(bool monoIsTracking);
	/// Notifier that the undulator tracking has changed
	void undulatorTrackingChanged(bool undulatorIsTracking);
	/// Notifier that the exit slit position tracking has changed
	void exitSlitTrackingChanged(bool exitSlitIsTracking);

	/// Notifier that the fluxResolutionGroup has changed
	void fluxResolutionGroupChanged();
	/// Notifier that the exit slit gap has changed
	void exitSlitGapChanged(double exitSlitGap);
	/// Notifier that the grating has changed
	void gratingChanged(SGMBeamlineInfo::sgmGrating grating);
	/// Notifier that the harmonic has changed
	void harmonicChanged(SGMBeamlineInfo::sgmHarmonic harmonic);

public slots:
	/// Sets the values for the trackingGroup
	void setTrackingGroup(const AMControlInfoList &trackingGroup);
	/// Sets the choice for mono tracking
	void setMonoTracking(bool monoIsTracking);
	/// Sets the choice for undulator tracking
	void setUndulatorTracking(bool undulatorIsTracking);
	/// Sets the choice for exit slit tracking
	void setExitSlitTracking(bool exitSlitIsTracking);

	/// Sets the values for the flux/resolution group
	void setFluxResolutionGroup(const AMControlInfoList &fluxResolutionGroup);
	/// Sets the value for the exit slit gap
	void setExitSlitGap(double exitSlitGapValue);
	/// Sets the choice for the grating
	void setGrating(SGMBeamlineInfo::sgmGrating gratingValue);
	/// Sets the choice for the harmonic
	void setHarmonic(SGMBeamlineInfo::sgmHarmonic harmonicValue);

protected:
	/// Store the tracking group to the database
	AMDbObject* dbGetTrackingGroup() { return &trackingGroup_; }
	/// Load the tracking group from the database: Never called, because dbGetTrackingGroup() is always valid.
	void dbLoadTrackingGroup(AMDbObject*) {} // never called

	/// Store the flux/resolution group to the database
	AMDbObject* dbGetFluxResolutionGroup() { return &fluxResolutionGroup_; }
	/// Load the flux/resolution group from the database: Never called, because dbGetFluxResolutionGroup() is always valid.
	void dbLoadFluxResolutionGroup(AMDbObject*) {} // never called

protected:
	/// Info list for the tracking group (track mono, track undulator, track exit slit)
	AMControlInfoList trackingGroup_;
	/// Info list for the flux/resolution group (exit slit gap, grating, harmonic)
	AMControlInfoList fluxResolutionGroup_;
};

#endif // SGMSSCANCONFIGURATIONDBOBJECT_H

