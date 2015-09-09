#ifndef BIOXASSCANCONFIGURATION_H
#define BIOXASSCANCONFIGURATION_H

#include "acquaman/BioXAS/BioXASScanConfigurationDbObject.h"
#include <QDebug>

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
	double energy() const { qDebug() << "Configuration energy:" << dbObject_->energy(); return dbObject_->energy(); }
	/// Returns the edge.
	QString edge() const { qDebug() << "Configuration edge:" << dbObject_->edge(); return dbObject_->edge(); }
	/// Returns the list of regions of interest.
	QList<AMRegionOfInterest *> regionsOfInterest() const { return dbObject_->regionsOfInterest(); }

	// Setters
	///////////////////////////////////////////

	/// Sets the energy.
	void setEnergy(double newEnergy) { qDebug() << "Configuration energy being set to:" << newEnergy; dbObject_->setEnergy(newEnergy); }
	/// Sets the edge.
	void setEdge(const QString &newEdge) { qDebug() << "Configuration edge being set to:" << newEdge; dbObject_->setEdge(newEdge); }
	/// Adds a region of interest to the list.
	void addRegionOfInterest(AMRegionOfInterest *region) { dbObject_->addRegionOfInterest(region); }
	/// Removes a region of interest from the list.
	void removeRegionOfInterest(AMRegionOfInterest *region) { dbObject_->removeRegionOfInterest(region); }

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
	/// Returns a string that displays all the regions of interest.
	QString regionsOfInterestHeaderString(const QList<AMRegionOfInterest *> &regions) const;

	/////////////////////////////////////////////

	/// The database object we're encapsulating.
	BioXASScanConfigurationDbObject *dbObject_;
};

#endif // BIOXASSCANCONFIGURATION_H
