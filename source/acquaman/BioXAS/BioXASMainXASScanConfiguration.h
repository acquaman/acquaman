#ifndef BIOXASMAINXASSCANCONFIGURATION_H
#define BIOXASMAINXASSCANCONFIGURATION_H

#include <QObject>

#include "acquaman/AMStepScanConfiguration.h"
#include "acquaman/BioXAS/BioXASScanConfiguration.h"

class BioXASMainXASScanConfiguration : public AMStepScanConfiguration, public BioXASScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(AMDbObject* configurationDbObject READ dbReadScanConfigurationDbObject WRITE dbWriteScanConfigurationDbObject)
	Q_PROPERTY(QString header READ headerText WRITE setHeaderText)

	Q_CLASSINFO("AMDbObject_Attributes", "description=BioXAS Main XAS Scan Configuration")

public:
	/// Constructor.
	Q_INVOKABLE BioXASMainXASScanConfiguration(QObject *parent = 0);
	/// Copy Constructor.
	BioXASMainXASScanConfiguration(const BioXASMainXASScanConfiguration &original);
	///Destructor.
	virtual ~BioXASMainXASScanConfiguration();
	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;
	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController();
	/// Returns a pointer to a newly-created AMScanConfigurationView that is appropriate for viewing and editing this kind of scan configuration. Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanConfigurationView* createView();
	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const;
	/// Get a nice looking string that contains all the standard information in an XAS scan.  Used when exporting.
	virtual QString headerText() const;
    /// Returns the current total estimated time for a scan to complete.
    double totalTime() const { return totalTime_; }
    /// Overloaded. Returns the current total estimated time but also specifies whether the time should be recomputed first.
    double totalTime(bool recompute) { if(recompute) computeTotalTimeImplementation(); return totalTime_; }

signals:
	/// Notifier that the total time estimate has changed.
	void totalTimeChanged(double);

public slots:

protected slots:
	/// Computes the total time any time the regions list changes.
	void computeTotalTime() { computeTotalTimeImplementation(); }
	/// Helper slot that connects the new region ot the computeTotalTime slot.
	void onRegionAdded(AMScanAxisRegion *region);
	/// Helper slot that disconnects the region from the computeTotalTime slot.
	void onRegionRemoved(AMScanAxisRegion *region);

protected:
	/// Method that does all the calculations for calculating the estimated scan time.
	virtual void computeTotalTimeImplementation();
    /// Holds the total time in seconds that the scan is estimated to take.
    double totalTime_;
};

#endif // BIOXASMAINXASSCANCONFIGURATION_H
