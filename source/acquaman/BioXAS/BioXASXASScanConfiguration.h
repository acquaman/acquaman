#ifndef BIOXASXASSCANCONFIGURATION_H
#define BIOXASXASSCANCONFIGURATION_H

#include "acquaman/AMGenericStepScanConfiguration.h"
#include "acquaman/BioXAS/BioXASScanConfiguration.h"

class BioXASXASScanConfiguration : public AMGenericStepScanConfiguration, public BioXASScanConfiguration
{
    Q_OBJECT

//	Q_PROPERTY(AMDbObject* configurationDbObject READ dbReadScanConfigurationDbObject WRITE dbWriteScanConfigurationDbObject)
//	Q_PROPERTY(QString header READ headerText WRITE setHeaderText)

public:
	/// Constructor.
	explicit BioXASXASScanConfiguration(QObject *parent = 0);
	/// Copy constructor.
	BioXASXASScanConfiguration(const BioXASXASScanConfiguration &original);
	/// Destructor.
	virtual ~BioXASXASScanConfiguration();

	/// Returns a description of the scan technique.
	virtual QString technique() const { return "XAS"; }
	/// Returns a description of this scan configuration.
	virtual QString description() const { return "XAS Scan"; }
	/// Returns a detailed description of this scan configuration.
	virtual QString detailedDescription() const { return "BioXAS XAS Scan"; }
	/// Returns the standard information for an XAS scan.  Used when exporting.
	virtual QString headerText() const;

//	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
//	virtual AMScanConfiguration* createCopy() const = 0;
//	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
//	virtual AMScanController* createController() = 0;
	/// Returns a pointer to a newly-created AMScanConfigurationView that is appropriate for viewing and editing this kind of scan configuration. Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanConfigurationView* createView();

public slots:
	/// Sets a controlInfo to an axis.  If the axis has no control associated with it yet, then it will add it to the list, otherwise it will replace it.
	virtual void setControl(AMControlInfo newInfo);
	/// Removes a control from the given axis.  Also removes the axis.
	virtual void removeControl();

protected slots:
	/// Helper slot that connects the new region ot the computeTotalTime slot.
	void onRegionAdded(AMScanAxisRegion *region);
	/// Helper slot that disconnects the region from the computeTotalTime slot.
	void onRegionRemoved(AMScanAxisRegion *region);
};

#endif // BIOXASXASSCANCONFIGURATION_H
