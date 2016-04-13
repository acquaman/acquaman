#ifndef SGMMAPSCANCONFIGURATION_H
#define SGMMAPSCANCONFIGURATION_H

#include "acquaman/AMGenericContinuousScanConfiguration.h"

/// This class implements SGM mapping with the hexapod.
class SGMMapScanConfiguration : public AMGenericContinuousScanConfiguration
{
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=SGM Spatial Map Continuous Scan")

public:
	/// Constructor.
	Q_INVOKABLE SGMMapScanConfiguration(QObject *parent = 0);
	/// Copy constructor.
	SGMMapScanConfiguration(const SGMMapScanConfiguration &other);
	/// Destructor.
	virtual ~SGMMapScanConfiguration();

	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration *createCopy() const;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController *createController();

	/// Returns a pointer to a newly-created AMScanConfigurationView that is appropriate for viewing and editing this kind of scan configuration. Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanConfigurationView *createView();

	/// Returns the technique string.
	virtual QString technique() const;
	/// A human-readable description of this scan configuration. Can be re-implemented to provide more details. Used by scan action to set the title for the action view.
	virtual QString description() const;
	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const;
};

#endif // SGMMAPSCANCONFIGURATION_H
