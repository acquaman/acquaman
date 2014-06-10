#ifndef SGMSCANCONFIGURATION2013_H
#define SGMSCANCONFIGURATION2013_H

#include "dataman/SGM/SGMBeamlineInfo.h"
#include "acquaman/SGM/SGMSScanConfigurationDbObject.h"

/*!
  This class is a non-QObject class that holds the database object for a generic scan configuration.  It has getters
  and setters to the database object.  This class exists purely because classes can not multiply inherit two QObject
  classes.
  */
class SGMScanConfiguration2013
{
public:
	/// Constructor
 	virtual ~SGMScanConfiguration2013();
	SGMScanConfiguration2013();
	/// Copy Constructor
	SGMScanConfiguration2013(const SGMScanConfiguration2013 &original);

	// DbObject specific methods.
	////////////////////////////////////////////

	/// Returns the database object.  Intended for gaining access to its signals.
	SGMSScanConfigurationDbObject *dbObject() const { return dbObject_; }
	/// The database reading member function.
	AMDbObject *dbReadScanConfigurationDbObject() { return dbObject_; }
	/// The database writing member function.
	void dbWriteScanConfigurationDbObject(AMDbObject *object);

	// Getters
	/////////////////////////////////////////

	/// Returns the current trackingGroup values
	const AMControlInfoList& trackingGroup() const { return dbObject_->trackingGroup(); }
	/// Returns the mono tracking value
	bool monoTracking() const { return dbObject_->monoTracking(); }
	/// Returns the undulator tracking value
	bool undulatorTracking() const { return dbObject_->undulatorTracking(); }
	/// Returns the exit slit position tracking value
	bool exitSlitTracking() const { return dbObject_->exitSlitTracking(); }

	/// Returns the current flux/resolution group values
	const AMControlInfoList& fluxResolutionGroup() const { return dbObject_->fluxResolutionGroup(); }
	/// Returns the exit slit gap value
	double exitSlitGap() const { return dbObject_->exitSlitGap(); }
	/// Returns the grating value
	SGMBeamlineInfo::sgmGrating grating() const { return dbObject_->grating(); }
	/// Returns the harmonic value
	SGMBeamlineInfo::sgmHarmonic harmonic() const { return dbObject_->harmonic(); }

	// Setters
	////////////////////////////////////////

	/// Sets the values for the trackingGroup
	void setTrackingGroup(const AMControlInfoList &trackingGroup) { dbObject_->setTrackingGroup(trackingGroup); }
	/// Sets the choice for mono tracking
	void setMonoTracking(bool monoIsTracking) { dbObject_->setMonoTracking(monoIsTracking); }
	/// Sets the choice for undulator tracking
	void setUndulatorTracking(bool undulatorIsTracking) { dbObject_->setUndulatorTracking(undulatorIsTracking); }
	/// Sets the choice for exit slit tracking
	void setExitSlitTracking(bool exitSlitIsTracking) { dbObject_->setExitSlitTracking(exitSlitIsTracking); }

	/// Sets the values for the flux/resolution group
	void setFluxResolutionGroup(const AMControlInfoList &fluxResolutionGroup) { dbObject_->setFluxResolutionGroup(fluxResolutionGroup); }
	/// Sets the value for the exit slit gap
	void setExitSlitGap(double exitSlitGapValue) { dbObject_->setExitSlitGap(exitSlitGapValue); }
	/// Sets the choice for the grating
	void setGrating(SGMBeamlineInfo::sgmGrating gratingValue) { dbObject_->setGrating(gratingValue); }
	/// Sets the choice for the harmonic
	void setHarmonic(SGMBeamlineInfo::sgmHarmonic harmonicValue) { dbObject_->setHarmonic(harmonicValue); }

protected:
	/// The database object we're encapsulating.
	SGMSScanConfigurationDbObject *dbObject_;
};

#endif // SGMSCANCONFIGURATION2013_H
