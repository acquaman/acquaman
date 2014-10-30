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
	Q_PROPERTY(double edgeEnergy READ edgeEnergy WRITE setEdgeEnergy)
	Q_PROPERTY(QString header READ headerText WRITE setHeaderText)
	Q_PROPERTY(double microprobeSampleStageX READ microprobeSampleStageX WRITE setMicroprobeSampleStageX)
	Q_PROPERTY(double microprobeSampleStageZ READ microprobeSampleStageZ WRITE setMicroprobeSampleStageZ)

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

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const;

	/// Returns the current edge energy in EV
	double edgeEnergy() const;

	/// Returns the name of the current edge.
	QString edge() const { return edge_; }

	/// Get a nice looking string that contains all the standard information in an XAS scan.   Used when exporting.
	virtual QString headerText() const;

	/// Returns the desired X position for the uProbe sample stage
	double microprobeSampleStageX() const;
	/// Returns the desired Z position for the uProbe sample stage
	double microprobeSampleStageZ() const;

signals:
	/// Emitted when the edge energy (in EV) changes
	void edgeEnergyChanged(double edgeEnergy);
	/// Emitted when the edge of interest (elemental edge name) changes
	void edgeChanged(const QString &edge);
	/// Notifier that the total time estimate has changed.
	void totalTimeChanged(double);

	/// Emitted when the desired X position is changed
	void microprobeSampleStageXChanged(double microprobeSampleStageX);
	/// Emitted when the desired Z position is changed
	void microprobeSampleStageZChanged(double microprobeSampleStageZ);

public slots:
	/// Sets the actual edge energy in EV
	void setEdgeEnergy(double edgeEnergy);

	/// Sets the current edge for the scan.
	void setEdge(QString edgeName);

	/// Sets the X position for the uProbe sample stage
	void setMicroprobeSampleStageX(double microprobeSampleStageX);
	/// Sets the Z position for the uProbe sample stage
	void setMicroprobeSampleStageZ(double microprobeSampleStageZ);

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

	/// Actual edge energy we're referring to
	double edgeEnergy_;
	/// The edge being scanned.
	QString edge_;

	/// The X position for the uProbe sample stage
	double microprobeSampleStageX_;
	/// The Z position for the uProbe sample stage
	double microprobeSampleStageZ_;
};

#endif // SXRMBEXAFSSCANCONFIGURATION_H
