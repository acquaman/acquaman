#ifndef AMEXPERIMENT_H
#define AMEXPERIMENT_H



#include "dataman/AMDbObject.h"
#include "dataman/AMDatabaseDefinition.h"
#include <QImage>

#include <QBuffer>
#include <QByteArray>
#include <QPixmap>


/// This class represents instances of objects that map to Experiments in the database of user information.
/*!
<b>Notes for subclassing AMDbObject, to create more detailed storable data types:</b>
- Include the Q_OBJECT macro so that Qt's meta-object system knows your class is a distinct type.
- You must re-implement metaDataKeys(), metaDataUniqueKeys(), and metaDataAllKeys(), calling the base class where appropriate.
- It's recommended to initialize your meta-data pieces (metaDataUniqueKeys()) inside metaData_ in the constructor.
- If you need to load or save anything more than your metaData_, you must re-implement loadFromDb() and storeToDb()
- If you want to store yourself anywhere but in the main object table, you must re-implement databaseTableName().  (For example, AMSamples overload databaseTableName() to return AMDatabaseDefinition::sampleTableName(), hence making sure that they are stored in a separate table.)
- If you want to have non-blank thumbnails, you must provide thumbnailCount() and thumbnail(int index).
*/

class AMExperiment : public AMDbObject
{
	Q_OBJECT
public:
	/// Default constructor. In it we initialize the extra fields we want to store inside metaData_.
	explicit AMExperiment(QObject *parent = 0);

	/// This constructor initializes an experiment with a given name.
	AMExperiment(const QString& experimentName, QObject* parent = 0);

	/// This constructor immediately loads a stored example from the database.
	AMExperiment(int databaseId, AMDatabase* database, QObject* parent = 0);

	// Implement the Meta-data system
	///////////////////////////

	/// We write this function to specify all of our unique pieces of meta-data (excluding those inherited from base classes -- ie: own only)
	static QList<AMMetaMetaData> metaDataUniqueKeys() {
		QList<AMMetaMetaData> rv;
		;
		rv << AMMetaMetaData(QVariant::String, "notes", true);


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


	/// This returns a string of notes/comments about this experiment.
	QString notes() const {
		return metaData("notes").toString();
	}

	// Database information and access:
	///////////////////////////////
	/// We want to store experiments in a separate table (so that it's easy to create relationships between experiments and scan objects).  Therefore, we reimplement databaseTableName():


	/// We aren't storing any special information outside of the metaData_ hash, so we can use the default implementations of loadFromDb() and storeToDb().


	// Thumbnail System
	/////////////////////////////////
	/// It would be nice to have (at least one) thumbnail... for now, it'll come from our sample image.
	int thumbnailCount() const {
			return 0;

	}
	QString databaseTableName() const {
		return AMDatabaseDefinition::experimentTableName();
	}


signals:

public slots:

	/// Convenience function to set the notes/comments on this sample
	void setNotes(const QString& notes) {
		setMetaData("notes", notes);
	}



protected:
	/// We have a picture of this  In the future,
	QImage sampleImage_;

};

#endif // AMEXPERIMENT_H

