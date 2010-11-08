#ifndef AMSAMPLE_H
#define AMSAMPLE_H

#include "dataman/AMDbObject.h"
#include <QImage>

#include <QBuffer>
#include <QByteArray>
#include <QPixmap>

/// This class represents instances of objects that map to Samples in the user database.
class AMSample : public AMDbObject
{
Q_OBJECT
	Q_PROPERTY(QDateTime dateTime READ dateTime WRITE setDateTime)
	Q_PROPERTY(QString notes READ notes WRITE setNotes)
	Q_PROPERTY(QByteArray image READ rawImage WRITE setRawImage)

	/// Do not reuse ids in the sample database... when deleting samples, we want any scans that reference their ids to become invalid; not to refer to new samples that would reuse the deleted ids.
	AM_DBOBJECTINFO("doNotReuseIds=true;description=Sample")

public:
	/// Default constructor.
	explicit AMSample(QObject *parent = 0);
	/// This constructor initializes a sample with a given name.
	AMSample(const QString& sampleName, QObject* parent = 0);
	/// This constructor immediately loads a stored sample from the database.
	AMSample(int databaseId, AMDatabase* database, QObject* parent = 0);

	// Sample management interface

	/// Remove this sample from the database. All scans on this sample will now have an invalid id for their sampleId(). (That's okay -- the sample editors understand an invalid sampleId and interpret this to mean 'no sample associated'. Sample ids are never reused, so there is no danger of interpreting an old invalid sampleId as different sample.)
	/*! \todo implement generically in AMDbObject sytem... */
	void destroySample() { destroySample(database(), id()); }
	static void destroySample(AMDatabase* db, int sampleId);



	// Convenient access methods for our meta-data:
	/////////////////////////

	/// Date/time when the sample was first entered into the system.
	QDateTime dateTime() const {
		return dateTime_;
	}

	/// This returns a string of notes/comments about this sample.
	QString notes() const {
		return notes_;
	}


	// Thumbnail System
	/////////////////////////////////
	/// It would be nice to have (at least one) thumbnail... for now, it'll come from our sample image.
	int thumbnailCount() const {
		if(image_.isEmpty())
			return 0;
		else
			return 1;
	}

	AMDbThumbnail thumbnail(int index) const {
		if(index != 0 || image_.isEmpty())
			return AMDbThumbnail(name(), dateTime().toString("MMM d (yyyy)"), AMDbThumbnail::InvalidType, QByteArray());


		QPixmap image;
		if(image.loadFromData(image_, "PNG"))
			return AMDbThumbnail(name(), dateTime().toString("MMM d (yyyy)"), image);
		else
			return AMDbThumbnail(name(), dateTime().toString("MMM d (yyyy)"), AMDbThumbnail::InvalidType, QByteArray());
	}

	QByteArray rawImage() const { return image_; }
	QImage image() const { return qimage_; }


signals:

public slots:

	/// Convenience function to set the creation date/time associated with this sample.  We call setMetaData(), since this takes care of emitting the metaDataChanged() signal for us.
	void setDateTime(const QDateTime& dateTime) {
		dateTime_ = dateTime;
		setModified(true);
	}

	/// Convenience function to set the notes/comments on this sample
	void setNotes(const QString& notes) {
		notes_ = notes;
		setModified(true);
	}

	/// Add a photograph or image of this sample. These will be used as thumbnails in the database views as well.
	void setImage(const QImage& sampleImage) {
		qimage_ = sampleImage;
		QBuffer boutput;
		boutput.open(QIODevice::WriteOnly);
		sampleImage.save(&boutput, "PNG");
		boutput.close();
		image_ = boutput.buffer();
	}
	/// Set image as raw PNG byte array
	void setRawImage(const QByteArray& rawPngImage) { image_ = rawPngImage; }

protected:
	/// Image for this sample
	QImage qimage_;
	/// Image stored as PNG byte array for persistent (db) storage
	QByteArray image_;

	QString notes_;
	QDateTime dateTime_;

};

#endif // AMSAMPLE_H
