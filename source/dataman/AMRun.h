#ifndef AMRUN_H
#define AMRUN_H


#include "dataman/AMDbObject.h"
#include "dataman/AMDatabaseDefinition.h"
#include <QImage>

#include <QBuffer>
#include <QByteArray>
#include <QPixmap>

/// This class represents instances of objects that map to Runs in the database of user information. It provides a simple example of how to subclass AMDbObject to create your own C++ object that can be easily stored in the database, and exploit the general meta-data management system.

/*!
<b>Notes for subclassing AMDbObject, to create more detailed storable data types:</b>
- Include the Q_OBJECT macro so that Qt's meta-object system knows your class is a distinct type.
- You must re-implement metaDataKeys(), metaDataUniqueKeys(), and metaDataAllKeys(), calling the base class where appropriate.
- It's recommended to initialize your meta-data pieces (metaDataUniqueKeys()) inside metaData_ in the constructor.
- If you need to load or save anything more than your metaData_, you must re-implement loadFromDb() and storeToDb()
- If you want to store yourself anywhere but in the main object table, you must re-implement databaseTableName().  (For example, AMRuns overload databaseTableName() to return AMDatabaseDefinition::RunTableName(), hence making sure that they are stored in a separate table.)
- If you want to have non-blank thumbnails, you must provide thumbnailCount() and thumbnail(int index).
*/
/* This Run class will need:
-name
-date and time
-end time
-notes
-location
*/

/// AMRun's happen at AMFacility's... This is a lightweight class to store facility information in the database.
class AMFacility : public AMDbObject
{
	Q_OBJECT
public:
	explicit AMFacility(const QString& shortDescription, const QString& longDescription, const QString& iconFileName = QString(), QObject* parent = 0)
		: AMDbObject(parent) {
		setName(shortDescription);	// ex: "SGM"
		metaData_["description"] = longDescription;	// ex: "Canadian Light Source SGM Beamline"
		metaData_["iconFileName"] = iconFileName;
	}

	/// Unique meta-data:
	static QList<AMMetaMetaData> metaDataUniqueKeys() {
		QList<AMMetaMetaData> rv;
		rv << AMMetaMetaData(QVariant::String, "description", true);
		rv << AMMetaMetaData(QVariant::String, "iconFileName", true);
		/// \todo There might be more useful information to put here...
		return rv;
	}
	/// All meta-data:
	static QList<AMMetaMetaData> metaDataKeys() {
		return AMDbObject::metaDataKeys() << metaDataUniqueKeys();
	}

	/// All meta-data:
	virtual QList<AMMetaMetaData> metaDataAllKeys() const {
		return this->metaDataKeys();
	}

	QString databaseTableName() const {
		return AMDatabaseDefinition::facilityTableName();
	}

	/// Long description of the facility (ex: "Canadian Light Source SGM Beamline")
	QString description() const {
		return metaData("description").toString();
	}
	QString iconFileName() const {
		return metaData("iconFileName").toString();
	}

	int thumbnailCount() const {
		if(iconFileName().isEmpty())
			return 0;
		else
			return 1;
	}

	AMDbThumbnail thumbnail(int index) const {
		Q_UNUSED(index)

		if(iconFileName().isEmpty())
			return AMDbThumbnail(name(), description());
		else
			return AMDbThumbnail(name(), description(), QPixmap(iconFileName()));
	}





protected:


};

class AMRun : public AMDbObject
{
	Q_OBJECT
public:
	/// Default constructor. In it we initialize the extra fields we want to store inside metaData_.
	explicit AMRun(QObject *parent = 0);

	/// This constructor initializes a run with a given name.
	AMRun(const QString& runName, QObject* parent = 0);

	/// This constructor immediately loads a stored run from the database.
	AMRun(int databaseId, AMDatabase* database, QObject* parent = 0);

	// Implement the Meta-data system
	///////////////////////////

	/// We write this function to specify all of our unique pieces of meta-data (excluding those inherited from base classes -- ie: own only)
	static QList<AMMetaMetaData> metaDataUniqueKeys() {
		QList<AMMetaMetaData> rv;
		// We have a DateTime field representing when this run was first created, when it was completed, a rich-text field for notes, and the location (link via facilityId) that this run took place.
		rv << AMMetaMetaData(QVariant::DateTime, "dateTime", true);
		rv << AMMetaMetaData(QVariant::String, "notes", true);
		rv << AMMetaMetaData(QVariant::Int, "facilityId", true);
		rv << AMMetaMetaData(QVariant::DateTime, "endDateTime", true);

		return rv;
	}


	/// This function needs to be overloaded to return all the available pieces of meta data for this type of object, including those inherited from base classes. (ie: own + base classes'). We simply append our unique meta-data onto the base class:
	static QList<AMMetaMetaData> metaDataKeys() {
		return AMDbObject::metaDataKeys() << metaDataUniqueKeys();
	}

	/// This virtual function returns all the available pieces of meta data for this type of object, by introspecting it's most detailed type. (ie: own + base classes' + subclasses')
	virtual QList<AMMetaMetaData> metaDataAllKeys() const {
		return this->metaDataKeys();
	}

	// Convenient access methods for our meta-data:
	/////////////////////////

	/// Although we can access everything through metaData() and setMetaData(), we can also write convenience functions for direct acces. This returns the creation date/time for this run.
	QDateTime dateTime() const {
		return metaData("dateTime").toDateTime();
	}

	/// This returns a string of notes/comments about this run.
	QString notes() const {
		return metaData("notes").toString();
	}

	/// This returns a string regarding the location of the run.
	int facilityId() const{
		return metaData("facilityId").toInt();
	}

	/// This returns the end of run date only
	QDateTime endDateTime() const{
		return metaData("endDateTime").toDateTime();
	}

	/// This function finds out how many scans are in this run.  (If this instance is not associated with any database yet, it returns 0)
	int scanCount() const;

	/// Returns a list of the id's of all scans in this run
	QList<int> scanIds() const;

	// Database information and access:
	///////////////////////////////
	/// We want to store runs in a separate table (so that it's easy to create relationships between runs and other objects).  Therefore, we reimplement databaseTableName():
	QString databaseTableName() const {
		return AMDatabaseDefinition::runTableName();
	}


	/// We aren't storing any special information outside of the metaData_ hash, so we can use the default implementations of loadFromDb() and storeToDb().


	// Thumbnail System
	/////////////////////////////////

	/// Runs don't have thumbnails...
	int thumbnailCount() const {
		return 0;
	}


signals:

public slots:

	/// Convenience function to set the creation date/time associated with this run.  We call setMetaData(), since this takes care of emitting the metaDataChanged() signal for us.
	void setDateTime(const QDateTime& dateTime) {
		setMetaData("dateTime", dateTime);
	}

	/// Convenience function to set the notes/comments on this run
	void setNotes(const QString& notes) {
		setMetaData("notes", notes);
	}

	/// Convenience function to set the end date for this run
	void setEndDate(const QDate& endDate) {
		setMetaData("endDate", endDate);
	}

	/// Convenience function to set the location for this run
	void setFacilityId(int facilityId){
		setMetaData("facilityId", facilityId);
	}



protected:


};



#endif // AMRUN_H
