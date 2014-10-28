#ifndef SXRMBEXAFSSCANCONFIGURATION_H
#define SXRMBEXAFSSCANCONFIGURATION_H

#include "acquaman/AMStepScanConfiguration.h"

class SXRMBEXAFSScanConfiguration : public AMStepScanConfiguration
{
Q_OBJECT
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

signals:
	/// Emitted when the edge energy (in EV) changes
	void edgeEnergyChanged(double edgeEnergy);
	/// Emitted when the edge of interest (elemental edge name) changes
	void edgeChanged(const QString &edge);

public slots:
	/// Sets the actual edge energy in EV
	void setEdgeEnergy(double edgeEnergy);

	/// Sets the current edge for the scan.
	void setEdge(QString edgeName);

protected:
	/// Actual edge energy we're referring to
	double edgeEnergy_;
	/// The edge being scanned.
	QString edge_;


};

#endif // SXRMBEXAFSSCANCONFIGURATION_H
