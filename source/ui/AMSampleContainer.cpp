#include "AMSampleContainer.h"

#include "AMSampleEthan.h"
#include "AMShapeDataSet.h"
#include "AMShapeData.h"

AMSampleContainer::AMSampleContainer(QObject* parent)
    :QAbstractListModel(parent)
{
    nullSample_ = new AMSampleEthan();
    nullSample_->setName("");
    nullSample_->setNotes("");
    nullSample_->setDateTime(QDateTime());
    nullSample_->setElementList(QList<int>());
    nullSample_->setRawImage(QByteArray());
    nullSample_->setSamplePlate(0);
    nullSample_->setScanList(QList<AMScan*>());
}

const QList<AMSampleEthan *> AMSampleContainer::sampleList() const
{
    return sampleList_;
}

AMSampleEthan *AMSampleContainer::sample(int index) const
{
    if(isValid(index))
    {
        return sampleList_[index];
    }
    return nullSample_;
}

QString AMSampleContainer::sampleName() const
{
    return sampleName(currentIndex());
}

QString AMSampleContainer::sampleName(int index) const
{
    return sample(index)->name();
}

QDateTime AMSampleContainer::dateTime() const
{
    return dateTime(currentIndex());
}

QDateTime AMSampleContainer::dateTime(int index) const
{
    return sample(index)->dateTime();
}

QString AMSampleContainer::dateTimeString() const
{
    return dateTimeString(currentIndex());
}

QString AMSampleContainer::dateTimeString(int index) const
{
    return sample(index)->dateTimeString();
}

QString AMSampleContainer::sampleNotes() const
{
    return sampleNotes(currentIndex());
}

QString AMSampleContainer::sampleNotes(int index) const
{
    return sample(index)->notes();
}

QByteArray AMSampleContainer::sampleImage() const
{
    return sampleImage(currentIndex());
}

QByteArray AMSampleContainer::sampleImage(int index) const
{
    return sample(index)->rawImage();
}

int AMSampleContainer::currentIndex() const
{
    if(sampleList_.count() < 1 || index_ < 0)
        return -1;
    return index_;
}

int AMSampleContainer::count() const
{
    return sampleList_.count();
}

int AMSampleContainer::rowCount(const QModelIndex &parent) const
{
    return count();
}

QVariant AMSampleContainer::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    switch(role)
    {
    case Qt::DisplayRole:
        return sampleList_.at(index.row())->name();
    default:
        return QVariant();
    }
}

int AMSampleContainer::indexOfSample(const AMShapeData *shape) const
{
    foreach(AMSampleEthan* sample, sampleList_)
    {
        if(sample->sampleShapePositionData() == shape)
            return sampleList_.indexOf(sample);
    }
    return -1;

}

void AMSampleContainer::setSampleList(QList<AMSampleEthan *> sampleList)
{
    sampleList_ = sampleList;
}

void AMSampleContainer::addSample(AMSampleEthan *sample)
{
    if(!sampleList_.contains(sample))
    {
        beginInsertRows(QModelIndex(), count(), count());
        sampleList_.append(sample);
        endInsertRows();
        connect(sample, SIGNAL(nameChanged(QString)), this, SLOT(updateNames()));
    }
}

void AMSampleContainer::removeSample(AMSampleEthan *sample)
{
    int counts = sampleList_.count(sample) - 1;
    if(counts < 0) return;
    beginRemoveRows(QModelIndex(), 0, counts);
    sampleList_.removeAll(sample);
    endRemoveRows();


}

void AMSampleContainer::setSampleName(QString name, int index)
{
    sample(index)->setName(name);
}

void AMSampleContainer::setSampleDateTime(QDateTime dateTime, int index)
{
    sample(index)->setDateTime(dateTime);
}

void AMSampleContainer::setSampleNotes(QString notes, int index)
{
    sample(index)->setNotes(notes);
}

void AMSampleContainer::setSampleImage(QByteArray image, int index)
{
    sample(index)->setRawImage(image);
}

void AMSampleContainer::setSampleImage(QImage image, int index)
{
    sample(index)->setImage(image);
}

void AMSampleContainer::setIndex(int index)
{
    index_ = index;
}

void AMSampleContainer::updateSamples()
{
    const QList<AMShapeData*> shapeList = AMShapeDataSet::set()->shapeList();
    if(shapeList.count() > sampleList().count())
    {
        foreach(AMShapeData* shape, shapeList)
        {
            if(!hasShape(shape))
            {
                AMSampleEthan* newSample = new AMSampleEthan();
                newSample->setSampleShapePositionData(shape);
                newSample->setName(shape->name());
                addSample(newSample);
            }
        }
    }
    else if(shapeList.count() < sampleList().count())
    {
        foreach(AMSampleEthan* sample, sampleList_)
        {
            if(!shapeList.contains(sample->sampleShapePositionData()))
            {
                removeSample(sample);
            }
        }
    }
}

void AMSampleContainer::updateNames()
{
    dataChanged(QModelIndex(),QModelIndex());
}


bool AMSampleContainer::isValid(int index) const
{
    return (index >= 0 && index < sampleList_.count());
}

bool AMSampleContainer::hasShape(AMShapeData *shapeData) const
{
    bool has = false;
    foreach(AMSampleEthan* sample, sampleList_)
    {
        if(sample->sampleShapePositionData() == shapeData)
            has = true;
    }
    return has;
}
