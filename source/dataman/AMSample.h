#ifndef AMSAMPLE_H
#define AMSAMPLE_H

#include "dataman/database/AMDbObject.h"
#include "util/AMElement.h"
#include "dataman/AMScan.h"
#include "dataman/AMSamplePlatePre2013.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "acquaman.h"
#include <QVector3D>
#include <QVector>

class AMShapeData;
class AMSamplePlate;

#define AMSAMPLE_EDITCURRENTTAG_TAG_NOT_FOUND 592001
#define AMSAMPLE_EDITCURRENTTAG_TAG_INDEX_OUT_OF_RANGE 592002
#define AMSAMPLE_REQUEST_TO_REMOVE_NULL_POSITIION_DATA 592003

/// Represents a sample to be scanned.  Holds the sample plate it is on, the scans
/// that have been performed on it, as well as an element list, notes, image, dateTime.
class AMSample : public AMDbObject
{
	Q_OBJECT
	Q_PROPERTY(QDateTime dateTime READ dateTime WRITE setDateTime)
	Q_PROPERTY(QString notes READ notes WRITE setNotes)
	Q_PROPERTY(QByteArray image READ rawImage WRITE setRawImage)
	Q_PROPERTY(QStringList tags READ tags WRITE setTags)
	Q_PROPERTY(AMIntList elementList READ elementList WRITE setElementList)
	Q_PROPERTY(AMQVector3DVector shapeData READ dbReadShapeData WRITE dbLoadShapeData)
	Q_PROPERTY(AMConstDbObjectList scanList READ dbReadScanList WRITE dbLoadScanList)
	Q_PROPERTY(AMConstDbObject* samplePlate READ dbReadSamplePlate WRITE dbLoadSamplePlate)

	Q_CLASSINFO("AMDbObject_Attributes", "doNotReuseIds=true;description=Sample")
	Q_CLASSINFO("elementIds", "hidden=true")
public:
	/// Default constructor
	Q_INVOKABLE explicit AMSample(QObject* parent = 0);
	/// This constructor initializes a sample with a given name.
	AMSample(const QString& sampleName, QObject* parent = 0);
	/// This constructor immediately loads a stored sample from the database.
	AMSample(int databaseId, AMDatabase *database, QObject *parent = 0);
	/// Destructor
	virtual ~AMSample();

	/// Returns the time of creation for this sample
	QDateTime dateTime() const;
	/// formatted string for dateTime
	QString dateTimeString() const;

	/// Returns any notes associated with this sample
	QString notes() const;

	/// If an image (picture) is available it returns it
	QByteArray image() const;

	/// Returns the elements associated with this sample
	QList<const AMElement*> elements() const;
	/// Returns the elements as a list of atomic numbers
	QList<int> elementList() const;
	/// returns a string list of the element symbols
	QString elementString() const;

	/// Returns a list of scans that were taken on this sample. Since the list is an AMConstDbObjectList, only const AMScan pointers can be returned.
	AMConstDbObjectList scanList() const;
	/// Returns a const pointer to the AMScan if the index is valid. Returns 0 otherwise
	const AMScan* scanAt(int index) const;
	/// Returns true if scan is in scanList_, else returns false
	bool hasScan(AMScan* scan) const;

	/// Returns a list of tags related to this sample
	const QStringList tags() const;
	/// Returns true if string is in tags_, else returns false
	bool hasTag(QString tag) const;

	/// Returns the shapeData (2D/3D representation of the position) for this sample
	AMShapeData* sampleShapePositionData() const;

	/// Thumbnails
	int thumbnailCount() const;
	AMDbThumbnail thumbnail(int index) const;

	/// Returns the constDbObject wrapping the sample plate (in case you want to check on the lazy loading status)
	AMConstDbObject* constDbSamplePlate() const;
	/// Returns a const pointer to the sample plate we're associated with
	const AMSamplePlate* samplePlate() const;

public slots:
	/// Sets the name of the sample
	void setName(const QString &name);

	/// Sets the creation time for the sample
	void setDateTime(const QDateTime dateTime);
	/// Sets dateTime_ to the current DateTime
	void setCurrentDateTime();

	/// Sets the notes field for the sample
	void setNotes(const QString notes);

	/// Set image from a QImage
	void setImage(const QImage& image);
	/// Set image from a raw QByteArray
	void setRawImage(const QByteArray& rawPngImage);

	/// Sets the entire elements list at once (overwriting any current values)
	void setElements(const QList<const AMElement*> elements);
	/// adds an element to the element list, if it is not already in it (adds it in order of atomic number)
	void addElement(const AMElement* element);
	/// removes the element from the list.
	void removeElement(const AMElement* element);
	/// if the element is in the list, removes it, else adds it
	void toggleElement(const AMElement* element);

	/// Sets the entire scan list (overwriting any current values)
	void setScanList(AMConstDbObjectList scanList);
	/// Adds a scan to the scanList, if it is not already in it
	void addScan(AMScan* scan);
	/// Removes all instances of a scan, if it is in the scanList, else does nothing
	void removeScan(AMScan* scan);

	/// Sets the entire tags list (overwriting any current values)
	void setTags(const QStringList tags);
	/// Adds a tag to the stringList, if it is not already in it
	void addTag(const QString tag);
	/// Removes all instances of a tag, if it is in the tag list, else does nothing
	void removeTag(const QString tag);
	/// set the current tag
	void setCurrentTag(QString tag);
	/// replaces the text of the current tag with the specified text
	void editCurrentTag(QString tag);

	/// Sets the element list from a list of atomic numbers (overwriting any current values)
	void setElementList(const QList<int>& elements);

	/// Sets the shape data for this sample
	void setSampleShapePositionData(AMShapeData* sampleShapePositionData);
	/// Removes the current shape data from this sample
	void removeSampleShapePositionData();

	/// Sets the sample plate to be associated with this sample
	void setSamplePlate(const AMSamplePlate *samplePlate);

	/// Removes the shape data and then emits a request signal
	void removeSample();

	/// Forces a store to database call on this sample
	void forceStoreToDb();

protected slots:
	/// Handles changes to the shape data
	void onShapeDataChanged();

signals:
	/// Emitted when the sample name changes
	void sampleNameChanged(const QString &name);

	/// Emited when the creation time is changed
	void dateTimeChanged(const QDateTime &dateTime);

	/// Emitted when the notes are changed
	void notesChanged(const QString &notes);

	/// Emitted when the list of tags is changed
	void tagsChanged(const QStringList &tags);
	/// Request for an update to current tag
	void requestCurrentTag();
	/// Emitted when the current tag changes
	void currentTagChanged(const QString &tag);

	/// Emitted when the list of elements is changed
	void elementsChanged(const QString &elementList);

	/// Emitted when the shape data for this sample changes
	void sampleShapeDataChanged();

	/// Emitted as a general signal to say "something about this sample has changed"
	void sampleDetailsChanged();

	/// Request signal emitted by calling removeSample()
	void sampleAboutToBeRemoved();

	/// Emitted to request that the sample plate store this sample to the database
	void requestStoreToDb();

protected:
	/// format string used to format the dateTime
	QString dateTimeFormat() const;
	/// initialization

	void init(QString name);
	/// sets up connections for AMSample
	void makeConnections();


	/// Returns the image as a QByteArray for database saving
	QByteArray rawImage() const;

	/// Called to save the scan list to the database
	AMConstDbObjectList dbReadScanList() const;
	/// Called to load the scan list from the database
	void dbLoadScanList(const AMConstDbObjectList& newScanList);

	/// Called to save the shape data to the database
	AMQVector3DVector dbReadShapeData() const;
	/// Called to load the shape data from the database
	void dbLoadShapeData(AMQVector3DVector newShapeData);

	/// Called to save the sample plate to the database
	AMConstDbObject* dbReadSamplePlate() const;
	/// Called to load the sample plate from the database
	void dbLoadSamplePlate(AMConstDbObject *newSamplePlate);

protected:
	/// The dateTime of the samples creation
	QDateTime dateTime_;
	/// Notes on the sample
	QString notes_;
	/// Image of the sample
	QByteArray image_;
	/// List of elements
	QList<const AMElement*> elements_;
	/// Tags for this sample
	QStringList tags_;
	/// The currently selected tag
	QString currentTag_;

	/// List of scans
	AMConstDbObjectList scanList_;

	/// A constDbObject referece to our sample plate
	AMConstDbObject *samplePlate_;

	/// The shape data (2D/3D) representation of this sample
	AMShapeData* sampleShapePositionData_;
};

#endif // AMSAMPLE_H
