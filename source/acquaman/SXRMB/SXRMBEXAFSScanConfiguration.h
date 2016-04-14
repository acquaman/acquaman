#ifndef SXRMBEXAFSSCANCONFIGURATION_H
#define SXRMBEXAFSSCANCONFIGURATION_H

#include "acquaman/AMStepScanConfiguration.h"
#include "acquaman/SXRMB/SXRMBScanConfiguration.h"

/// This class holds the scan configuration of a standard EXAFS/XAS scan.
class SXRMBEXAFSScanConfiguration : public AMStepScanConfiguration, public SXRMBScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(AMDbObject* configurationDbObject READ dbReadScanConfigurationDbObject WRITE dbWriteScanConfigurationDbObject)
	Q_PROPERTY(QString edge READ edge WRITE setEdge)
	Q_PROPERTY(QString header READ headerText WRITE setHeaderText)

	Q_CLASSINFO("AMDbObject_Attributes", "description=SXRMB EXAFS Scan Configuration")

public:
	/// Constructor
	Q_INVOKABLE SXRMBEXAFSScanConfiguration(QObject *parent = 0);
	/// Copy Constructor.
	SXRMBEXAFSScanConfiguration(const SXRMBEXAFSScanConfiguration &original);

	/// Destructor
	virtual ~SXRMBEXAFSScanConfiguration();

	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController();

	/// Returns a pointer to a newly-created AMScanConfigurationView that is appropriate for viewing and editing this kind of scan configuration. Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanConfigurationView* createView();

	/// Returns the technique string.
	virtual QString technique() const;
	/// A human-readable description of this scan configuration. Can be re-implemented to provide more details. Used by scan action to set the title for the action view.
	virtual QString description() const;
	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const;
	/// Returns whether this is an XAS technique scan
	virtual bool isXASScan() const { return true; }

	/// Returns the name of the current edge.
	QString edge() const { return edge_; }

	/// Get a nice looking string that contains all the standard information in an XAS scan.   Used when exporting.
	virtual QString headerText() const;

signals:
	/// Emitted when the edge of interest (elemental edge name) changes
	void edgeChanged(const QString &edge);
	/// Notifier that the total time estimate has changed.
	void totalTimeChanged(double);

public slots:
	/// Sets the current edge for the scan.
	void setEdge(QString edgeName);

protected slots:
	/// Computes the total time any time the regions list changes.
	void computeTotalTime() { computeTotalTimeImplementation(); }
	/// Helper slot that connects the new region to the computeTotalTime slot.
	void onRegionAdded(AMScanAxisRegion *region);
	/// Helper slot that disconnects the region from the computTotalTime slot.
	void onRegionRemoved(AMScanAxisRegion *region);

protected:
	/// Method that does all the calculations for calculating the estimated scan time.
	virtual void computeTotalTimeImplementation();

	/// The edge being scanned.
	QString edge_;
};

#endif // SXRMBEXAFSSCANCONFIGURATION_H
