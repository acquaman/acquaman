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

signals:
	/// Notifier that the energy has changed.
	void energyChanged(double);
	/// Notifier that the edge has changed.
	void edgeChanged(const QString &);

public slots:
	/// Sets the energy.
	void setEnergy(double newEnergy);
	/// Sets the edge.
	void setEdge(const QString &newEdge);

protected slots:

protected:
	/// The energy used for this scan.
	double energy_;
	/// The edge associated with this scan.
	QString edge_;
};

#endif // BIOXASSCANCONFIGURATIONDBOBJECT_H
