#include "dataman/AMSample.h"

#include "util/AMPeriodicTable.h"

#include "beamline/camera/AMShapeData.h"

AMSample::AMSample(QObject* parent)
	: AMDbObject(parent)
{

	init("New Sample");
}

AMSample::AMSample(const QString &sampleName, QObject *parent)
	: AMDbObject(parent)
{

	init(sampleName);
}

AMSample::AMSample(int databaseId, AMDatabase *database, QObject *parent)
	: AMDbObject(parent)
{
	loadFromDb(database, databaseId);
}

AMSample::~AMSample(){
	if(sampleShapePositionData_){
		sampleShapePositionData_->deleteLater();
		sampleShapePositionData_ = 0;
	}
}

void AMSample::destroySample()
{
	destroySample(database(),id());
}

void AMSample::destroySample(AMDatabase *db, int sampleId)
{
	if(db)
		db->deleteRow(sampleId,AMDbObjectSupport::s()->tableNameForClass<AMSample>());
}

QDateTime AMSample::dateTime() const
{
	return dateTime_;
}

QString AMSample::dateTimeString() const
{
	/// returns the dateTime formatted with dateTimeFormat
	return dateTime().toString(dateTimeFormat());
}

QString AMSample::notes() const
{
	return notes_;
}

QByteArray AMSample::image() const
{
	return rawImage();
}

QList<const AMElement *> AMSample::elements() const
{
	return elements_;
}

QList<AMScan *> AMSample::scanList() const
{
	return scanList_;
}

const QStringList AMSample::tags() const
{
	return tags_;
}

AMSamplePlatePre2013 *AMSample::samplePlate() const
{
	return samplePlate_;
}

AMDbObject *AMSample::dbGetSamplePlate() const
{
	return (AMDbObject*)samplePlate_;
}

QString AMSample::samplePlateName() const
{
	if(samplePlate_)
		return samplePlate_->name();
	else return "";
}

QList<int> AMSample::elementList() const
{
	QList<int> elementList;
	foreach(const AMElement* element, elements_)
	{
		if(element)
		{
			elementList<<element->atomicNumber();
		}
	}
	return elementList;
}

AMShapeData *AMSample::sampleShapePositionData() const
{
	return sampleShapePositionData_;
}

AMDbObjectList AMSample::dbReadScanList() const
{
	AMDbObjectList scanList;
	foreach(AMScan* scan, scanList_)
	{
		scanList<<scan;
	}
	return scanList;
}


int AMSample::thumbnailCount() const
{
	if(image_.isEmpty()) return 0;
	return 1;
}

AMDbThumbnail AMSample::thumbnail(int index) const
{
	bool success = true;
	if(index != 0 || !thumbnailCount())
		success = false;

	QImage image;
	if(success && image.loadFromData(image_, "PNG"))
		return AMDbThumbnail(name(),dateTimeString(),image);
	else
		return AMDbThumbnail(name(), dateTimeString(), AMDbThumbnail::InvalidType, QByteArray());
}

QByteArray AMSample::rawImage() const
{
	return image_;
}

/// checks to see if the tag is in the list
bool AMSample::hasTag(QString tag) const
{
	return tags_.contains(tag,Qt::CaseSensitive);
}

/// checks to see if the scan is in the list
bool AMSample::hasScan(AMScan *scan) const
{
	return scanList_.contains(scan);
}


QString AMSample::elementString() const
{
	/// output a string listing of the elements
	QStringList elementStringList;
	foreach(const AMElement* element, elements_)
	{
		elementStringList.append(element->symbol());
	}

	return elementStringList.join(", ");
}

void AMSample::setName(const QString &name)
{
	AMDbObject::setName(name);
	AMShapeData* samplePosition = sampleShapePositionData();
	if(samplePosition)
	{
		samplePosition->setName(name);
	}
	emit sampleDetailsChanged();
}

void AMSample::setDateTime(const QDateTime dateTime)
{

	dateTime_ = dateTime;
	setModified(true);
}

void AMSample::setNotes(const QString notes)
{
	if(notes_ != notes){
		notes_ = notes;
		setModified(true);
		emit notesChanged(notes_);
		emit sampleDetailsChanged();
	}
}

/// sets the image from a QImage
void AMSample::setImage(const QImage& sampleImage)
{
	QBuffer imageBuffer;
	imageBuffer.open(QIODevice::WriteOnly);
	sampleImage.save(&imageBuffer, "PNG");
	imageBuffer.close();
	image_ = imageBuffer.buffer();
	setModified(true);
	emit sampleDetailsChanged();
}

void AMSample::setRawImage(const QByteArray &rawPngImage)
{
	image_ = rawPngImage;
	setModified(true);
	emit sampleDetailsChanged();
}

void AMSample::setElements(const QList<const AMElement *> elements)
{
	elements_ = elements;
	setModified(true);
	emit sampleDetailsChanged();
}

void AMSample::setScanList(const QList<AMScan *> scanList)
{
	scanList_ = scanList;
	setModified(true);
	emit sampleDetailsChanged();
}

void AMSample::setTags(const QStringList tags)
{
	if(tags_ != tags){
		tags_ = tags;
		setModified(true);
		emit tagsChanged(tags_);
		emit sampleDetailsChanged();
	}
}

void AMSample::setSamplePlate(AMSamplePlatePre2013 *samplePlate)
{
	samplePlate_ = samplePlate;
	setModified(true);
}

void AMSample::dbSetSamplePlate(AMDbObject *samplePlate)
{
	AMSamplePlatePre2013* newPlate = (AMSamplePlatePre2013*)samplePlate;
	setSamplePlate(newPlate);
}

void AMSample::dbLoadScanList(const AMDbObjectList &newScanList)
{
	while(!scanList_.isEmpty())
	{
		AMScan* deleteMe = scanList_.takeLast();
		delete deleteMe;
	}

	foreach(AMDbObject* newScan, newScanList)
	{
		AMScan* scan = qobject_cast<AMScan*>(newScan);
		if(scan)
		{
			scanList_.append(scan);
		}
		else
			qDebug()<<"AMSampleEthan::dbLoadScanList - could not load scan list from db";
	}

}

void AMSample::setElementList(const AMIntList& elements)
{
	elements_.clear();
	foreach(int element, elements)
	{
		addElement(AMPeriodicTable::table()->elementByAtomicNumber(element));
	}
	emit sampleDetailsChanged();
	emit elementsChanged(elementString());
}

void AMSample::setSampleShapePositionData(AMShapeData *sampleShapePositionData)
{
    if(sampleShapePositionData_ != sampleShapePositionData)
    {
        if(sampleShapePositionData_)
        {
            disconnect(sampleShapePositionData_, SIGNAL(nameChanged(QString)), this, SLOT(setName(QString)));
            disconnect(this, SIGNAL(currentTagChanged(QString)), sampleShapePositionData_, SLOT(setOtherDataFieldOne(QString)));
            disconnect(this, SIGNAL(elementsChanged(QString)), sampleShapePositionData_, SLOT(setOtherDataFieldTwo(QString)));
			disconnect(sampleShapePositionData_, SIGNAL(shapeDataChanged(AMShapeData*)), this, SLOT(onShapeDataChanged()));
        }
        sampleShapePositionData_ = sampleShapePositionData;
        if(sampleShapePositionData_)
        {
            sampleShapePositionData_->setName(name());
			if(!tags_.isEmpty())
				setCurrentTag(tags_.first());
            connect(sampleShapePositionData_, SIGNAL(nameChanged(QString)), this, SLOT(setName(QString)));
            // set other Data field one to tags
            connect(this, SIGNAL(currentTagChanged(QString)), sampleShapePositionData_, SLOT(setOtherDataFieldOne(QString)));
            connect(this, SIGNAL(elementsChanged(QString)), sampleShapePositionData_, SLOT(setOtherDataFieldTwo(QString)));
			connect(sampleShapePositionData_, SIGNAL(shapeDataChanged(AMShapeData*)), this, SLOT(onShapeDataChanged()));
        }

		emit sampleShapeDataChanged();
		emit requestCurrentTag();
		emit currentTagChanged(currentTag_);
		emit elementsChanged(elementString());
    }
}

/// deletes the sample shape position data.
void AMSample::removeSampleShapePositionData()
{
    qDebug()<<"AMSample::removeSampleShapePositionData";
    if(sampleShapePositionData_)
    {

        sampleShapePositionData()->removeShape();
        setSampleShapePositionData(0);
    }
    else
    {
        qDebug()<<"AMSample::removeSampleShapePositionData - sample shape is null.";
    }
}


void AMSample::addTag(const QString tag)
{
	/// add a tag, if not already in the list
	bool modified = false;
	if(!hasTag(tag))
	{
		tags_.append(tag);
		modified = true;
	}

	/// remove all empty tags
	int count = tags_.removeAll("");
	if(count > 0) modified = true;
	setModified(modified);
	if(modified)
	{
		emit tagsChanged(tags_);
		emit sampleDetailsChanged();
	}
}

void AMSample::removeTag(const QString tag)
{
	/// remove all instances of tag
	int count = tags_.removeAll(tag);
	if(count > 0)
	{
		setModified(true);
		emit tagsChanged(tags_);
		emit sampleDetailsChanged();
	}

}

void AMSample::addScan(AMScan *scan)
{
	/// add scan to the list
	if(!hasScan(scan))
	{
		scanList_.append(scan);
		setModified(true);
	}
}

void AMSample::removeScan(AMScan *scan)
{
	/// remove all instances of scan
	while(hasScan(scan))
	{
		scanList_.removeAt(scanList_.indexOf(scan));
		setModified(true);
	}
}

void AMSample::addElement(const AMElement *element)
{
	/// add an element into the list.
	/// place it in order according to atomic number
	if(!elements_.contains(element))
	{
		int position = 0;
		while(elements_.count() > position && elements_.at(position)->atomicNumber() < element->atomicNumber())
			position++;
		elements_.insert(position,element);
		setModified(true);
	}
	emit sampleDetailsChanged();
	emit elementsChanged(elementString());

}

void AMSample::removeElement(const AMElement *element)
{
	/// removes all instances of element from the list
	int count = elements_.removeAll(element);
	if(count > 0) setModified(true);
	emit sampleDetailsChanged();
	emit elementsChanged(elementString());
}

void AMSample::toggleElement(const AMElement *element)
{
	/// toggle element into or out of the list
	if(elements_.contains(element))
		removeElement(element);
	else
		addElement(element);
}

void AMSample::setCurrentDateTime()
{
	setDateTime(QDateTime::currentDateTime());
	setModified(true);
	emit dateTimeChanged(dateTime_);
	emit sampleDetailsChanged();
}

void AMSample::setCurrentTag(QString tag)
{

	currentTag_ = tag;
	emit currentTagChanged(currentTag_);
}

void AMSample::getCurrentTag()
{
	emit requestCurrentTag();
}

void AMSample::editCurrentTag(QString tag)
{
	QString oldTag = currentTag_;
	int index = tags_.indexOf(oldTag);
	if(index == -1)
	{
		qDebug()<<"AMSample::editCurrentTag - Cannot edit tag, tag not found.";
	}
	else if(index >= 0 && index < tags_.size())
	{
		tags_.removeAt(index);
		tags_.insert(index, tag);
		emit tagsChanged(tags_);
		emit sampleDetailsChanged();
	}
	else
	{
		qDebug()<<"AMSample::editCurrentTag - index out of range.";
	}



}

void AMSample::removeSample()
{
    qDebug()<<"AMSample::removeSample";
        removeSampleShapePositionData();
	emit sampleAboutToBeRemoved();
}

void AMSample::onShapeDataChanged()
{
	setModified(true);
}


/// the format used to present dateTime as a string
QString AMSample::dateTimeFormat() const
{
	return QString("MMM d (yyyy)");
}

void AMSample::init(QString name)
{
	makeConnections();
	sampleShapePositionData_ = 0;
	currentTag_ = "";
	setName(name);
	setCurrentDateTime();
	setSamplePlate(new AMSamplePlatePre2013());


}

void AMSample::makeConnections()
{
	connect(this, SIGNAL(tagsChanged(QStringList)), this, SLOT(getCurrentTag()));
	connect(this, SIGNAL(nameChanged(QString)), this, SIGNAL(sampleDetailsChanged()));
}


AMQVector3DVector AMSample::dbReadShapeData() const{
	if(sampleShapePositionData_)
		return sampleShapePositionData_->coordinates();
	return QVector<QVector3D>();
}

void AMSample::dbLoadShapeData(AMQVector3DVector newShapeData){
	AMShapeData* shapeData = new AMShapeData(this);
	setSampleShapePositionData(shapeData);
	sampleShapePositionData_->setCoordinateShape(newShapeData);
        emit sampleShapeDataChanged();
}


