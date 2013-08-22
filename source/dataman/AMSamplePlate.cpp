#include "AMSamplePlate.h"

#include "util/AMDateTimeUtils.h"

AMSamplePlate::AMSamplePlate(QObject *parent) :
	AMDbObject(parent)
{
	dateTime_ = QDateTime::currentDateTime();

	// Forward internal signals (itemAdded, etc.) from our signalSource() as our own
	connect(samples_.signalSource(), SIGNAL(itemAboutToBeAdded(int)), this, SIGNAL(sampleAboutToBeAdded(int)));
	connect(samples_.signalSource(), SIGNAL(itemAdded(int)), this, SIGNAL(sampleAdded(int)));
	connect(samples_.signalSource(), SIGNAL(itemAboutToBeRemoved(int)), this, SIGNAL(sampleAboutToBeRemoved(int)));
	connect(samples_.signalSource(), SIGNAL(itemRemoved(int)), this, SIGNAL(sampleRemoved(int)));
	connect(samples_.signalSource(), SIGNAL(itemChanged(int)), this, SIGNAL(sampleChanged(int)));
}

QDateTime AMSamplePlate::dateTime() const{
	return dateTime_;
}

int AMSamplePlate::sampleCount() const{
	return samples_.count();
}

AMSample* AMSamplePlate::sampleAt(int index){
	if(index == -1)
		return samples_.at(samples_.count()-1);
	if(index < 0 || index > sampleCount())
		return 0; //NULL
	return samples_.at(index);
}

bool AMSamplePlate::addSample(AMSample *sample){
	samples_.append(sample);
	connect(sample, SIGNAL(sampleDetailsChanged()), this, SLOT(onSampleDetailsChanged()));
	qDebug() << "Adding sample named " << sample->name() << " and storing to db";
	storeToDb(database());
	return true;
}

int AMSamplePlate::indexOfSample(AMSample *sample){
	if(!sample)
		return -1;
	for(int x = 0; x < samples_.count(); x++)
		if(samples_.at(x) == sample)
			return x;
	return -1;
}

#include "beamline/camera/AMSampleCamera.h"
void AMSamplePlate::onSampleCameraShapesChanged(){
	const QList<AMShapeData*> shapeList = AMSampleCamera::set()->shapeList();
	if(shapeList.count() > sampleCount())
	{
		foreach(AMShapeData* shape, shapeList)
		{
			if(!sampleFromShape(shape))
			{
				AMSample* newSample = new AMSample();
				newSample->setSampleShapePositionData(shape);
				newSample->setName(shape->name());
				addSample(newSample);
				emit sampleAddedThroughCamera(newSample);
			}
		}
	}
	/*
	else if(shapeList.count() < sampleList().count())
	{
		foreach(AMSample* sample, sampleList_)
		{
			if(!shapeList.contains(sample->sampleShapePositionData()))
			{
				removeSample(sample);
			}
		}
	}
	*/
}

void AMSamplePlate::onShapeDataPropertyUpdated(AMShapeData *shapeData){
	AMSample *sample = sampleFromShape(shapeData);
	if(sample){
		qDebug() << "The sample said it made an explicit update, so storeToDb";
		sample->storeToDb(database());
	}
}

void AMSamplePlate::onSampleDetailsChanged(){
	AMSample *sample = qobject_cast<AMSample*>(QObject::sender());
	if(sample){
		qDebug() << "It's a sample, so let's emit sampleChanged";
		emit sampleChanged(indexOfSample(sample));
	}
}

AMSample* AMSamplePlate::sampleFromShape(AMShapeData *shapeData){
	for(int x = 0; x < sampleCount(); x++)
		if(sampleAt(x)->sampleShapePositionData() == shapeData)
			return sampleAt(x);
	return 0; //NULL
}

void AMSamplePlate::dbLoadDateTime(const QDateTime &newDateTime){
	dateTime_ = newDateTime;
}

AMDbObjectList AMSamplePlate::dbGetSamples(){
	AMDbObjectList retVal;
	for(int x = 0; x < samples_.count(); x++)
		retVal << samples_[x];
	return retVal;
}

void AMSamplePlate::dbLoadSamples(const AMDbObjectList &newSamples){
	samples_.clear();

	for(int x = 0; x < newSamples.count(); x++){
		AMSample *newSample = qobject_cast<AMSample*>(newSamples.at(x));
		if(newSample)
			samples_.append(newSample);
	}
}


AMSamplePlateBrowser::AMSamplePlateBrowser(AMDatabase *database, QObject *parent) :
	QAbstractListModel(parent)
{
	database_ = database;
	reloadFromDatabase();
}

int AMSamplePlateBrowser::rowCount(const QModelIndex &parent) const
{
	if(parent.isValid())
		return 0;
	return allSamplePlates_.count();
}

QVariant AMSamplePlateBrowser::data(const QModelIndex &index, int role) const
{
	if(index.parent().isValid() || index.column() > 0)
		return QVariant();

	switch(role){
	case Qt::DisplayRole:
		return QString("%1 (created %2)").arg(allSamplePlates_.at(index.row())->name()).arg(AMDateTimeUtils::prettyDateTime(allSamplePlates_.at(index.row())->dateTime()));
	case AM::PointerRole:
		return QVariant::fromValue((QObject*)(allSamplePlates_.at(index.row())));
	default:
		return QVariant();
	}
}

Qt::ItemFlags AMSamplePlateBrowser::flags(const QModelIndex &index) const
{
	Q_UNUSED(index)
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant AMSamplePlateBrowser::headerData(int section, Qt::Orientation orientation, int role) const
{
	Q_UNUSED(section)
	if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return QString("Sample Plates");
	else
		return QVariant();
}

void AMSamplePlateBrowser::reloadFromDatabase(){
	allSamplePlates_.clear();
	if(database_){
		QList<int> allSamplePlateIds = database_->objectsWhere(AMDbObjectSupport::s()->tableNameForClass<AMSamplePlate>());
		AMSamplePlate *tempPlate;
		for(int x = 0; x < allSamplePlateIds.count(); x++){
			tempPlate = new AMSamplePlate();
			tempPlate->loadFromDb(database_, allSamplePlateIds.at(x));
			qDebug() << "Reloaded a sample plate named " << tempPlate->name() << " with count " << tempPlate->sampleCount();
			allSamplePlates_.append(tempPlate);
		}
	}
}
