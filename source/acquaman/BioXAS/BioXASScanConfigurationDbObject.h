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
	Q_PROPERTY(bool exportSpectraPreference READ exportSpectraPreference WRITE setExportSpectraPreference)
	Q_PROPERTY(bool exportSpectra READ exportSpectra WRITE setExportSpectra)

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

	/// Returns the preference for whether the XRF detector spectra are exported.
	bool exportSpectraPreference() const { return exportSpectraPreference_; }
	/// Returns true if XRF detector spectra are exported.
	bool exportSpectra() const { return exportSpectra_; }

signals:
	/// Notifier that the energy has changed.
	void energyChanged(double);
	/// Notifier that the edge has changed.
	void edgeChanged(const QString &);
	/// Notifier that the preference for whether the XRF detector spectra are exported has changed.
	void exportSpectraPreferenceChanged(bool spectraExported);
	/// Notifier that the flag indicating whether the spectra are exported has changed.
	void exportSpectraChanged(bool spectraExported);

public slots:
	/// Sets the energy.
	void setEnergy(double newEnergy);
	/// Sets the edge.
	void setEdge(const QString &newEdge);

	/// Sets the preference for exporting XRF detector spectra.
	void setExportSpectraPreference(bool spectraExported);
	/// Sets the flag for whether the XRF detector spectra are exported.
	void setExportSpectra(bool spectraExported);

protected:
	/// The energy used for this scan.
	double energy_;
	/// The edge associated with this scan.
	QString edge_;

	/// The preference for exporting XRF detector spectra.
	bool exportSpectraPreference_;
	/// The flag for whether the XRF detector spectra are exported.
	bool exportSpectra_;
};

#endif // BIOXASSCANCONFIGURATIONDBOBJECT_H
