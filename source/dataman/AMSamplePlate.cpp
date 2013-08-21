#include "AMSamplePlate.h"

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

void AMSamplePlate::onSampleDetailsChanged(){
	AMSample *sample = qobject_cast<AMSample*>(QObject::sender());
	if(sample){
		qDebug() << "It's a sample, so let's emit sampleChanged";
		emit sampleChanged(indexOfSample(sample));
	}
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
		return allSamplePlates_.at(index.row())->name();
	default:
		return QVariant();
	}
}

Qt::ItemFlags AMSamplePlateBrowser::flags(const QModelIndex &index) const
{
	Q_UNUSED(index)
	return Qt::ItemIsEnabled;
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
			allSamplePlates_.append(tempPlate);
		}
	}
}
