#ifndef AMSAMPLE_H
#define AMSAMPLE_H

#include "dataman/AMDbObject.h"
#include "dataman/AMDatabaseDefinition.h"
#include <QImage>

#include <QBuffer>
#include <QByteArray>
#include <QPixmap>

/// This class represents instances of objects that map to Samples in the database of user information. It provides a simple example of how to subclass AMDbObject to create your own C++ object that can be easily stored in the database, and exploit the general meta-data management system.

/*!
<b>Notes for subclassing AMDbObject, to create more detailed storable data types:</b>
- Include the Q_OBJECT macro so that Qt's meta-object system knows your class is a distinct type.
- You must re-implement metaDataKeys(), metaDataUniqueKeys(), and metaDataAllKeys(), calling the base class where appropriate.
- It's recommended to initialize your meta-data pieces (metaDataUniqueKeys()) inside metaData_ in the constructor.
- If you need to load or save anything more than your metaData_, you must re-implement loadFromDb() and storeToDb()
- If you want to store yourself anywhere but in the main object table, you must re-implement databaseTableName().  (For example, AMSamples overload databaseTableName() to return AMDatabaseDefinition::sampleTableName(), hence making sure that they are stored in a separate table.)
- If you want to have non-blank thumbnails, you must provide thumbnailCount() and thumbnail(int index).
*/

class AMSample : public AMDbObject
{
Q_OBJECT
public:
	/// Default constructor. In it we initialize the extra fields we want to store inside metaData_.
	explicit AMSample(QObject *parent = 0);
	/// This constructor initializes a sample with a given name.
	AMSample(const QString& sampleName, QObject* parent = 0);
	/// This constructor immediately loads a stored sample from the database.
	AMSample(int databaseId, AMDatabase* database, QObject* parent = 0);

	// Implement the Meta-data system
	///////////////////////////

	/// We write this function to specify all of our unique pieces of meta-data (excluding those inherited from base classes -- ie: own only)
	static QList<AMMetaMetaData> metaDataUniqueKeys() {
		QList<AMMetaMetaData> rv;
		// We have a DateTime field representing when this sample was first created, and a rich-text field for notes about this sample.  We also have some pictures/thumbnail(s); the full-size image is stored here, and the thumbnail stored separately in the thumbnail system.
		rv << AMMetaMetaData(QVariant::DateTime, "dateTime", true);
		rv << AMMetaMetaData(QVariant::String, "notes", true);
		// todo: beamline information/sample positions?  Part of a separate sample plate/sample relationship?

		/// \todo In future versions, it would be nice to have an arbitrary set of images of this sample, including micrographs or digital pictures, along with meta-data for each (time of picture, etc.)
		rv << AMMetaMetaData(QVariant::ByteArray, "image", true);
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

	/// Although we can access everything through metaData() and setMetaData(), we can also write convenience functions for direct acces. This returns the creation date/time for this sample.
	QDateTime dateTime() const {
		return metaData("dateTime").toDateTime();
	}

	/// This returns a string of notes/comments about this sample.
	QString notes() const {
		return metaData("notes").toString();
	}

	// Database information and access:
	///////////////////////////////
	/// We want to store samples in a separate table (so that it's easy to create relationships between samples and scan objects).  Therefore, we reimplement databaseTableName():
	virtual QString databaseTableName() const {
		return AMDatabaseDefinition::sampleTableName();
	}

	/// We aren't storing any special information outside of the metaData_ hash, so we can use the default implementations of loadFromDb() and storeToDb().


	// Thumbnail System
	/////////////////////////////////
	/// It would be nice to have (at least one) thumbnail... for now, it'll come from our sample image.
	int thumbnailCount() const {
		if(metaData("image").isNull())
			return 0;
		else
			return 1;
	}

	AMDbThumbnail thumbnail(int index) const {
		if(index != 0 || metaData("image").isNull())
			return AMDbThumbnail(name(), dateTime().toString("MMM d (2010)"), AMDbThumbnail::InvalidType, QByteArray());


		QImage image;
		QByteArray imageByteArray = metaData("image").toByteArray();
		if(image.loadFromData(imageByteArray, "PNG")) {

			QImage thumbnailImage = image.scaled(QSize(240,180), Qt::KeepAspectRatio, Qt::SmoothTransformation);
			QBuffer boutput;
			boutput.open(QIODevice::WriteOnly);
			thumbnailImage.save(&boutput, "PNG");
			boutput.close();
			return AMDbThumbnail(name(), dateTime().toString("MMM d (2010)"), AMDbThumbnail::PNGType, boutput.buffer());
		}

		else
			return AMDbThumbnail(name(), dateTime().toString("MMM d (2010)"), AMDbThumbnail::InvalidType, QByteArray());
	}


signals:

public slots:

	/// Convenience function to set the creation date/time associated with this sample.  We call setMetaData(), since this takes care of emitting the metaDataChanged() signal for us.
	void setDateTime(const QDateTime& dateTime) {
		setMetaData("dateTime", dateTime);
	}

	/// Convenience function to set the notes/comments on this sample
	void setNotes(const QString& notes) {
		setMetaData("notes", notes);
	}

	/// Add a photograph or image of this sample. These will be used as thumbnails in the database views as well.
	void setImage(const QImage& sampleImage) {
		QBuffer boutput;
		boutput.open(QIODevice::WriteOnly);
		sampleImage.save(&boutput, "PNG");
		boutput.close();
		setMetaData("image", boutput.buffer());
	}

protected:
	/// We have a picture of this  In the future,
	QImage sampleImage_;

};

#endif // AMSAMPLE_H
