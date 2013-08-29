#ifndef AMSAMPLE_H
#define AMSAMPLE_H

#include "dataman/database/AMDbObject.h"
#include "util/AMElement.h"
#include "dataman/AMScan.h"
#include "dataman/AMSamplePlatePre2013.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "source/acquaman.h"
#include <QVector3D>
#include <QVector>


class AMShapeData;

//typedef QVector<QVector3D> AMQVector3DVector;
//Q_DECLARE_METATYPE(AMQVector3DVector)

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
	Q_PROPERTY(AMDbObject* samplePlate READ dbGetSamplePlate WRITE dbSetSamplePlate)
	Q_PROPERTY(AMDbObjectList scanList READ dbReadScanList WRITE dbLoadScanList)
	Q_PROPERTY(AMQVector3DVector shapeData READ dbReadShapeData WRITE dbLoadShapeData)

	Q_CLASSINFO("AMDbObject_Attributes", "doNotReuseIds=true;description=Sample")
	Q_CLASSINFO("elementIds", "hidden=true")
public:
	/// default constructor
	Q_INVOKABLE explicit AMSample(QObject* parent = 0);
	/// This constructor initializes a sample with a given name.
	AMSample(const QString& sampleName, QObject* parent = 0);
	/// this constructor immediately loads a stored sample from the database.
	AMSample(int databaseId, AMDatabase *database, QObject *parent = 0);

	virtual ~AMSample();


	/// sample management interface
	void destroySample();
	static void destroySample(AMDatabase* db, int sampleId);

	/// accessors
	QDateTime dateTime() const;
	/// formatted string for dateTime
	QString dateTimeString() const;
	QString notes() const;
	QByteArray image() const;
	QList<const AMElement*> elements() const;
	QList<AMScan*> scanList() const;
    const QStringList tags() const;
	AMSamplePlatePre2013* samplePlate() const;
	AMDbObject* dbGetSamplePlate() const;
	QString samplePlateName() const;
	/// returns the elements as a list of atomic numbers
	QList<int> elementList() const;
	AMShapeData* sampleShapePositionData() const;
	AMDbObjectList dbReadScanList() const;


	/// thumbnails
	int thumbnailCount() const;
	AMDbThumbnail thumbnail(int index) const;
	QByteArray rawImage() const;



	/// returns true if string is in tags_, else returns false
	bool hasTag(QString tag) const;
	/// returns true if scan is in scanList_, else returns false
	bool hasScan(AMScan* scan) const;
	/// returns a string list of the element symbols
	QString elementString() const;


public slots:

	/// mutators
	void setName(const QString &name);
	void setDateTime(const QDateTime dateTime);
	void setNotes(const QString notes);
	/// set Image from a QImage
	void setImage(const QImage& image);
	void setRawImage(const QByteArray& rawPngImage);
	void setElements(const QList<const AMElement*> elements);
	void setScanList(const QList<AMScan*> scanList);
	void setTags(const QStringList tags);
	void setSamplePlate(AMSamplePlatePre2013* samplePlate);
	void dbSetSamplePlate(AMDbObject* samplePlate);
	void dbLoadScanList(const AMDbObjectList& newScanList);

	/// sets the element list from a list of atomic numbers
	void setElementList(const QList<int>& elements);

	void setSampleShapePositionData(AMShapeData* sampleShapePositionData);

	/// adds a tag to the stringList, if it is not already in it
	void addTag(const QString tag);
	/// removes all instances of a tag, if it is in the tag list, else does nothing
	void removeTag(const QString tag);

	/// adds a scan to the scanList, if it is not already in it
	void addScan(AMScan* scan);
	/// removes all instances of a scan, if it is in the scanList, else does nothing
	void removeScan(AMScan* scan);

	/// adds an element to the element list, if it is not already in it.
	/// adds it in order of atomic number
	void addElement(const AMElement* element);
	/// removes the element from the list.
	void removeElement(const AMElement* element);
	/// if the element is in the list, removes it, else adds it
	void toggleElement(const AMElement* element);

	/// sets dateTime_ to the current DateTime
	void setCurrentDateTime();

	/// set the current tag
	void setCurrentTag(QString tag);
	void getCurrentTag();
	/// replaces the text of the current tag with the specified text
	void editCurrentTag(QString tag);

	void removeSample();


signals:
	void sampleNameChanged(const QString &name);
	void dateTimeChanged(const QDateTime &dateTime);
	void notesChanged(const QString &notes);
	void tagsChanged(const QStringList &tags);
	/// request for an update to current tag
	void requestCurrentTag();
	void currentTagChanged(const QString &tag);
	void elementsChanged(const QString &elementList);
	void sampleShapeDataChanged();

	void sampleDetailsChanged();

	void sampleAboutToBeRemoved();




protected:
	/// format string used to format the dateTime
	QString dateTimeFormat() const;
	/// initialization
	void init(QString name);
	/// sets up connections for AMSample
	void makeConnections();
	/// attempts to get current tag, otherwise returns first tag in list

	AMQVector3DVector dbReadShapeData() const;
	void dbLoadShapeData(AMQVector3DVector newShapeData);

protected:

	/// the dateTime of the samples creation
	QDateTime dateTime_;
	/// notes on the sample
	QString notes_;
	/// image of the sample
	QByteArray image_;
	/// list of elements
	QList<const AMElement*> elements_;
	/// list of scans
	QList<AMScan*> scanList_;
	/// tags for this sample
	QStringList tags_;
	/// sample plate this sample is associated with
	AMSamplePlatePre2013* samplePlate_;

	AMShapeData* sampleShapePositionData_;
	QString currentTag_;


};

#endif // AMSAMPLE_H
