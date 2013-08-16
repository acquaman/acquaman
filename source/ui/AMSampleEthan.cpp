#include "AMSampleEthan.h"

#include "util/AMPeriodicTable.h"

#include "AMShapeData.h"

AMSampleEthan::AMSampleEthan(QObject* parent)
    : AMDbObject(parent)
{
    setName("New Sample");
    setCurrentDateTime();
    setSamplePlate(new AMSamplePlate());
}

AMSampleEthan::AMSampleEthan(const QString &sampleName, QObject *parent)
    : AMDbObject(parent)
{
    setCurrentDateTime();
    setName(sampleName);
    setSamplePlate(new AMSamplePlate());
}

AMSampleEthan::AMSampleEthan(int databaseId, AMDatabase *database, QObject *parent)
    : AMDbObject(parent)
{
    loadFromDb(database, databaseId);
}

void AMSampleEthan::destroySample()
{
    destroySample(database(),id());
}

void AMSampleEthan::destroySample(AMDatabase *db, int sampleId)
{
    if(db)
        db->deleteRow(sampleId,AMDbObjectSupport::s()->tableNameForClass<AMSampleEthan>());
}

QDateTime AMSampleEthan::dateTime() const
{
    return dateTime_;
}

QString AMSampleEthan::dateTimeString() const
{
    /// returns the dateTime formatted with dateTimeFormat
    return dateTime().toString(dateTimeFormat());
}

QString AMSampleEthan::notes() const
{
    return notes_;
}

QByteArray AMSampleEthan::image() const
{
    return rawImage();
}

QList<const AMElement *> AMSampleEthan::elements() const
{
    return elements_;
}

QList<AMScan *> AMSampleEthan::scanList() const
{
    return scanList_;
}

QStringList AMSampleEthan::tags() const
{
    return tags_;
}

AMSamplePlate *AMSampleEthan::samplePlate() const
{
    return samplePlate_;
}

AMDbObject *AMSampleEthan::dbGetSamplePlate() const
{
    return (AMDbObject*)samplePlate_;
}

QString AMSampleEthan::samplePlateName() const
{
    if(samplePlate_)
        return samplePlate_->name();
    else return "";
}

QList<int> AMSampleEthan::elementList() const
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

AMShapeData *AMSampleEthan::sampleShapePositionData() const
{
    return sampleShapePositionData_;
}

AMDbObjectList AMSampleEthan::dbReadScanList() const
{
    AMDbObjectList scanList;
    foreach(AMScan* scan, scanList_)
    {
        scanList<<scan;
    }
    return scanList;
}


int AMSampleEthan::thumbnailCount() const
{
    if(image_.isEmpty()) return 0;
    return 1;
}

AMDbThumbnail AMSampleEthan::thumbnail(int index) const
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

QByteArray AMSampleEthan::rawImage() const
{
    return image_;
}

/// checks to see if the tag is in the list
bool AMSampleEthan::hasTag(QString tag) const
{
    return tags_.contains(tag,Qt::CaseSensitive);
}

/// checks to see if the scan is in the list
bool AMSampleEthan::hasScan(AMScan *scan) const
{
    return scanList_.contains(scan);
}


QString AMSampleEthan::elementString() const
{
    /// output a string listing of the elements
    QStringList elementStringList;
    foreach(const AMElement* element, elements_)
    {
        elementStringList.append(element->symbol());
    }

    return elementStringList.join(", ");
}

void AMSampleEthan::setDateTime(const QDateTime dateTime)
{
    dateTime_ = dateTime;
    setModified(true);
}

void AMSampleEthan::setNotes(const QString notes)
{
    notes_ = notes;
    setModified(true);
}

/// sets the image from a QImage
void AMSampleEthan::setImage(const QImage& sampleImage)
{
    QBuffer imageBuffer;
    imageBuffer.open(QIODevice::WriteOnly);
    sampleImage.save(&imageBuffer, "PNG");
    imageBuffer.close();
    image_ = imageBuffer.buffer();
    setModified(true);
}

void AMSampleEthan::setRawImage(const QByteArray &rawPngImage)
{
    image_ = rawPngImage;
    setModified(true);
}

void AMSampleEthan::setElements(const QList<const AMElement *> elements)
{
    elements_ = elements;
    setModified(true);
}

void AMSampleEthan::setScanList(const QList<AMScan *> scanList)
{
    scanList_ = scanList;
    setModified(true);
}

void AMSampleEthan::setTags(const QStringList tags)
{
    tags_ = tags;
    setModified(true);
}

void AMSampleEthan::setSamplePlate(AMSamplePlate *samplePlate)
{
    samplePlate_ = samplePlate;
    setModified(true);
}

void AMSampleEthan::dbSetSamplePlate(AMDbObject *samplePlate)
{
    AMSamplePlate* newPlate = (AMSamplePlate*)samplePlate;
    setSamplePlate(newPlate);
}

void AMSampleEthan::dbLoadScanList(const AMDbObjectList &newScanList)
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

void AMSampleEthan::setElementList(const AMIntList& elements)
{
    elements_.clear();
    foreach(int element, elements)
    {
        addElement(AMPeriodicTable::table()->elementByAtomicNumber(element));
    }
}

void AMSampleEthan::setSampleShapePositionData(AMShapeData *sampleShapePositionData)
{
    sampleShapePositionData_ = sampleShapePositionData;
}


void AMSampleEthan::addTag(const QString tag)
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

void AMSampleEthan::removeTag(const QString tag)
{
    /// remove all instances of tag
    int count = tags_.removeAll(tag);
    if(count > 0)
        setModified(true);
}

void AMSampleEthan::addScan(AMScan *scan)
{
    /// add scan to the list
    if(!hasScan(scan))
    {
        scanList_.append(scan);
        setModified(true);
    }
}

void AMSampleEthan::removeScan(AMScan *scan)
{
    /// remove all instances of scan
    while(hasScan(scan))
    {
        scanList_.removeAt(scanList_.indexOf(scan));
        setModified(true);
    }
}

void AMSampleEthan::addElement(const AMElement *element)
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

void AMSampleEthan::removeElement(const AMElement *element)
{
    /// removes all instances of element from the list
    int count = elements_.removeAll(element);
    if(count > 0) setModified(true);
}

void AMSampleEthan::toggleElement(const AMElement *element)
{
    /// toggle element into or out of the list
    if(elements_.contains(element))
        removeElement(element);
    else
        addElement(element);
}

void AMSampleEthan::setCurrentDateTime()
{
    setDateTime(QDateTime::currentDateTime());
    setModified(true);
}

/// the format used to present dateTime as a string
QString AMSampleEthan::dateTimeFormat() const
{
    return QString("MMM d (yyyy)");
}
