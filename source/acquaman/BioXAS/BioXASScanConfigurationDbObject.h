#ifndef BIOXASSCANCONFIGURATIONDBOBJECT_H
#define BIOXASSCANCONFIGURATIONDBOBJECT_H

#include "dataman/database/AMDbObject.h"
#include "dataman/AMRegionOfInterest.h"

/// This is the common configuration database object that shares meta information across all scan types.  For the time being there is only the edge, the energy, and the regions of interest.
class BioXASScanConfigurationDbObject : public AMDbObject
{
	Q_OBJECT

	Q_PROPERTY(double energy READ energy WRITE setEnergy)
	Q_PROPERTY(QString edge READ edge WRITE setEdge)
	Q_PROPERTY(bool usingXRFDetector READ usingXRFDetector WRITE setUsingXRFDetector)

	Q_CLASSINFO("usingXRFDetector", "upgradeDefault=false")

	Q_CLASSINFO("AMDbObject_Attributes", "description=BioXAS Scan Configuration Database Object")

public:
	/// Constructor.
	Q_INVOKABLE BioXASScanConfigurationDbObject(QObject *parent = 0);
	/// Copy constructor.
	BioXASScanConfigurationDbObject(const BioXASScanConfigurationDbObject &original);
	/// Destructor.
	virtual ~BioXASScanConfigurationDbObject();

	/// Returns the energy.
	double energy() const { return energy_; }
	/// Returns the edge.
	QString edge() const { return edge_; }
	/// Returns whether the scan is using an XRF detector.
	bool usingXRFDetector() const { return usingXRFDetector_; }
	/// Returns the regions of interest.
	QList<AMRegionOfInterest *> regionsOfInterest() const { return regionsOfInterest_; }

signals:
	/// Notifier that the energy has changed.
	void energyChanged(double);
	/// Notifier that the edge has changed.
	void edgeChanged(const QString &);
	/// Notifier that whether the scan is using an XRF detector or not has changed.
	void usingXRFDetectorChanged(bool);

public slots:
	/// Sets the energy.
	void setEnergy(double newEnergy);
	/// Sets the edge.
	void setEdge(const QString &newEdge);
	/// Sets whether the configuration is using an XRF detector or not.
	void setUsingXRFDetector(bool hasXRF);
	/// Adds a region of interest to the list.
	void addRegionOfInterest(AMRegionOfInterest *region);
	/// Removes a region of interest from the list.
	void removeRegionOfInterest(AMRegionOfInterest *region);

protected:
	/// Returns the regions of interest list.
	AMDbObjectList dbReadRegionsOfInterest();
	/// Called by the dtabase system on loadFromDb() to give us our new list of AMRegionOfInterest.
	void dbLoadRegionsOfInterest(const AMDbObjectList &newRegions);

	/// The energy used for this scan.
	double energy_;
	/// The edge associated with this scan.
	QString edge_;
	/// Flag noting whether an XRF detector is being used.
	bool usingXRFDetector_;
	/// The list of the regions of interest.
	QList<AMRegionOfInterest *> regionsOfInterest_;
};

#endif // BIOXASSCANCONFIGURATIONDBOBJECT_H
