#include "AMSampleEthan.h"

AMSampleEthan::AMSampleEthan(QObject* parent)
    : AMDbObject(parent)
{
}

AMSampleEthan::AMSampleEthan(const QString &sampleName, QObject *parent)
    : AMDbObject(parent)
{
    setCurrentDateTime();
    setName(sampleName);
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
    if(db ==0) return;

    db->deleteRow(sampleId,AMDbObjectSupport::s()->tableNameForClass<AMSampleEthan>());
}

QDateTime AMSampleEthan::dateTime() const
{
    return dateTime_;
}

QString AMSampleEthan::dateTimeString() const
{
    return dateTime().toString("MMM d  (yyyy)");
}

QString AMSampleEthan::notes() const
{
    return notes_;
}

QByteArray AMSampleEthan::image() const
{
    return rawImage();
}

QList<AMElement *> AMSampleEthan::elements() const
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

bool AMSampleEthan::hasTag(QString tag) const
{
    return tags_.contains(tag,Qt::CaseSensitive);
}

bool AMSampleEthan::hasScan(AMScan *scan) const
{
    return scanList_.contains(scan);
}

QString AMSampleEthan::elementString() const
{
    QStringList elementStringList;
    foreach(AMElement* element, elements_)
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

void AMSampleEthan::setElements(const QList<AMElement *> elements)
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

void AMSampleEthan::addTag(const QString tag)
{
    if(!hasTag(tag))
    {
        tags_.append(tag);
        setModified(true);
    }
    while(hasTag(""))
    {
        tags_.removeAt(tags_.indexOf(""));
    }
}

void AMSampleEthan::removeTag(const QString tag)
{
    while(hasTag(tag))
    {
        tags_.removeAt(tags_.indexOf(tag));
        setModified(true);
    }
}

void AMSampleEthan::addScan(AMScan *scan)
{
    if(!hasScan(scan))
    {
        scanList_.append(scan);
        setModified(true);
    }
}

void AMSampleEthan::removeScan(AMScan *scan)
{
    while(hasScan(scan))
    {
        scanList_.removeAt(scanList_.indexOf(scan));
        setModified(true);
    }
}

void AMSampleEthan::setCurrentDateTime()
{
    setDateTime(QDateTime::currentDateTime());
    setModified(true);
}
