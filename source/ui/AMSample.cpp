#include "AMSample.h"

#include "util/AMPeriodicTable.h"

#include "AMShapeData.h"

AMSample::AMSample(QObject* parent)
    : AMDbObject(parent)
{
    sampleShapePositionData_ = 0;
    setName("New Sample");
    setCurrentDateTime();
    setSamplePlate(new AMSamplePlatePre2013());
}

AMSample::AMSample(const QString &sampleName, QObject *parent)
    : AMDbObject(parent)
{
    setCurrentDateTime();
    setName(sampleName);
    setSamplePlate(new AMSamplePlatePre2013());
}

AMSample::AMSample(int databaseId, AMDatabase *database, QObject *parent)
    : AMDbObject(parent)
{
    loadFromDb(database, databaseId);
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

QStringList AMSample::tags() const
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
        samplePosition->setName(name);
}

void AMSample::setDateTime(const QDateTime dateTime)
{
    dateTime_ = dateTime;
    setModified(true);
}

void AMSample::setNotes(const QString notes)
{
    notes_ = notes;
    setModified(true);
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
}

void AMSample::setRawImage(const QByteArray &rawPngImage)
{
    image_ = rawPngImage;
    setModified(true);
}

void AMSample::setElements(const QList<const AMElement *> elements)
{
    elements_ = elements;
    setModified(true);
}

void AMSample::setScanList(const QList<AMScan *> scanList)
{
    scanList_ = scanList;
    setModified(true);
}

void AMSample::setTags(const QStringList tags)
{
    tags_ = tags;
    setModified(true);
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
}

void AMSample::setSampleShapePositionData(AMShapeData *sampleShapePositionData)
{
    sampleShapePositionData_ = sampleShapePositionData;
    sampleShapePositionData_->setName(name());
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
}

void AMSample::removeTag(const QString tag)
{
    /// remove all instances of tag
    int count = tags_.removeAll(tag);
    if(count > 0)
        setModified(true);
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

}

void AMSample::removeElement(const AMElement *element)
{
    /// removes all instances of element from the list
    int count = elements_.removeAll(element);
    if(count > 0) setModified(true);
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
}

/// the format used to present dateTime as a string
QString AMSample::dateTimeFormat() const
{
    return QString("MMM d (yyyy)");
}
