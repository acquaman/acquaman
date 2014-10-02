/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AMSAMPLEPRE2013_H
#define AMSAMPLEPRE2013_H

#include "dataman/database/AMDbObject.h"
#include <QImage>

#include <QBuffer>
#include <QByteArray>
#include <QImage>

/// This class represents instances of objects that map to Samples in the user database.
class AMSamplePre2013 : public AMDbObject
{
Q_OBJECT
	Q_PROPERTY(QDateTime dateTime READ dateTime WRITE setDateTime)
	Q_PROPERTY(QString notes READ notes WRITE setNotes)
	Q_PROPERTY(QByteArray image READ rawImage WRITE setRawImage)
	/// \todo This should be an indexed related table, so we can quickly find all the samples that contain a certain element.
	Q_PROPERTY(AMIntList elementList READ elementList WRITE setElementList)

	/// Do not reuse ids in the sample database... when deleting samples, we want any scans that reference their ids to become invalid. Not to refer to new samples that would reuse the deleted ids.
	Q_CLASSINFO("AMDbObject_Attributes", "doNotReuseIds=true;description=Sample")
	Q_CLASSINFO("elementIds", "hidden=true")

public:
	/// Default constructor.
	Q_INVOKABLE explicit AMSamplePre2013(QObject *parent = 0);
	/// This constructor initializes a sample with a given name.
	AMSamplePre2013(const QString& sampleName, QObject* parent = 0);
	/// This constructor immediately loads a stored sample from the database.
	AMSamplePre2013(int databaseId, AMDatabase* database, QObject* parent = 0);

	// Sample management interface

	/// Remove this sample from the database. All scans on this sample will now have an invalid id for their sampleId(). (That's okay -- the sample editors understand an invalid sampleId and interpret this to mean 'no sample associated'. Sample ids are never reused, so there is no danger of interpreting an old invalid sampleId as different sample.)
	/*! \todo implement generically in AMDbObject sytem... */
	void destroySample() { destroySample(database(), id()); }
	static void destroySample(AMDatabase* db, int sampleId);



	/// Convenience static function to return the sample name corresponding to a \c sampleId in \c database.  Returns "[Invalid Sample]" if not found.
	/*! Often we want to grab the sample name corresponding to a sampleId, but it's not necesary to load all the fields for the sample; this offers better performance than calling loadFromDb() just to access .name(). */
	static QString sampleNameForId(AMDatabase* db, int sampleId);



	// Metadata access
	/////////////////////////

	/// Date/time when the sample was first entered into the system.
	QDateTime dateTime() const {
		return dateTime_;
	}

	/// This returns a string of notes/comments about this sample.
	QString notes() const {
		return notes_;
	}

	/// Returns a list of the elements the user has declared to be in this sample, by atomic number. ex: Hydrogen is 1, Helium is 2, etc.)
	QList<int> elementList() const { return elements_; }

	/// Format the elements into a string: ex: "B, N, Cl"
	QString elementString() const;



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


		QImage image;
		if(image.loadFromData(image_, "PNG"))
			return AMDbThumbnail(name(), dateTime().toString("MMM d (yyyy)"), image);
		else
			return AMDbThumbnail(name(), dateTime().toString("MMM d (yyyy)"), AMDbThumbnail::InvalidType, QByteArray());
	}

	QByteArray rawImage() const { return image_; }

signals:

public slots:


	// Metadata access
	/////////////////////////

	/// Set the creation date/time associated with this sample.  We call setMetaData(), since this takes care of emitting the metaDataChanged() signal for us.
	void setDateTime(const QDateTime& dateTime) {
		dateTime_ = dateTime;
		setModified(true);
	}

	/// Set the notes/comments on this sample
	void setNotes(const QString& notes) {
		notes_ = notes;
		setModified(true);
	}

	/// Set the complete list of elements which are in this sample. Elements are specified atomic number.
	void setElementList(const QList<int>& elements) { elements_ = elements; setModified(true); }


	/// Add a photograph or image of this sample. These will be used as thumbnails in the database views as well.
	void setImage(const QImage& sampleImage) {
		QBuffer boutput;
		boutput.open(QIODevice::WriteOnly);
		sampleImage.save(&boutput, "PNG");
		boutput.close();
		image_ = boutput.buffer();
	}
	/// Set image as raw PNG byte array
	void setRawImage(const QByteArray& rawPngImage) { image_ = rawPngImage; }

protected:
	/// Image stored as PNG byte array for persistent (db) storage
	QByteArray image_;

	QString notes_;
	QDateTime dateTime_;
	AMIntList elements_;
};

#endif // AMSAMPLEPRE2013_H
