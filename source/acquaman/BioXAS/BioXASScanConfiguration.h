#ifndef BIOXASSCANCONFIGURATION_H
#define BIOXASSCANCONFIGURATION_H

#include "acquaman/BioXAS/BioXASScanConfigurationDbObject.h"

/*!
  This class is a non-QObject class that holds the database object for generic
  BioXAS scan details.  This class exists to bypass the limitation that you
  can't multiply inherit QObjects.
  */
class BioXASScanConfiguration
{

public:
	/// Constructor.
	BioXASScanConfiguration();
	/// Copy constructor.
	BioXASScanConfiguration(const BioXASScanConfiguration &original);
	/// Destructor.
	virtual ~BioXASScanConfiguration();

	// DbObject specific methods.
	////////////////////////////////////////////

	/// Returns the database object.  Intended for gaining access to its signals.
	BioXASScanConfigurationDbObject *dbObject() const { return dbObject_; }

	// Getters
	/////////////////////////////////////////////

	/// Returns the energy.
	double energy() const { return dbObject_->energy(); }
	/// Returns the edge.
	QString edge() const { return dbObject_->edge(); }
	/// Returns the collect spectra preference.
	bool collectSpectraPreference() const { return dbObject_->collectSpectraPreference(); }
	/// Returns the collect ICR preference.
	bool collectICRPreference() const { return dbObject_->collectICRPreference(); }

	/// Returns a string representation of the scan configuration.
	virtual QString toString() const;

	// Setters
	///////////////////////////////////////////

	/// Sets the energy.
	void setEnergy(double newEnergy) { dbObject_->setEnergy(newEnergy); }
	/// Sets the edge.
	void setEdge(const QString &newEdge) { dbObject_->setEdge(newEdge); }
	/// Sets the export spectra preference.
	void setCollectSpectraPreference(bool collectSpectra) { dbObject_->setCollectSpectraPreference(collectSpectra); }
	/// Sets the export ICR preference.
	void setCollectICRPreference(bool collectICR) { dbObject_->setCollectICRPreference(collectICR); }

protected:
	/// The database reading member function.
	AMDbObject *dbReadScanConfigurationDbObject() { return dbObject_; }
	/// The database writing member function.
	void dbWriteScanConfigurationDbObject(AMDbObject *object);

	// Header method and helper methods.
	////////////////////////////////////////

	/// Get a nice looking string that contains all the standard information in an XAS scan.   Used when exporting.
	virtual QString headerText() const = 0;
	/// This function does nothing.  It is there to preserve the fact that the database needs to be able to read and write.
	void setHeaderText(QString) {}

	/////////////////////////////////////////////

	/// The database object we're encapsulating.
	BioXASScanConfigurationDbObject *dbObject_;
};

#endif // BIOXASSCANCONFIGURATION_H
