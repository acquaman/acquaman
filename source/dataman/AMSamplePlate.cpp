#include "AMSamplePlate.h"

AMSamplePlate::AMSamplePlate(QObject *parent) :
	AMDbObject(parent), AMOrderedSet<QString, AMSample*>()
{
	setAllowsDuplicateKeys(true);

	dateTime_ = QDateTime::currentDateTime();

	// Forward internal signals (itemAdded, etc.) from our signalSource() as our own
	connect(signalSource(), SIGNAL(itemAboutToBeAdded(int)), this, SIGNAL(sampleAboutToBeAdded(int)));
	connect(signalSource(), SIGNAL(itemAdded(int)), this, SIGNAL(sampleAdded(int)));
	connect(signalSource(), SIGNAL(itemAboutToBeRemoved(int)), this, SIGNAL(sampleAboutToBeRemoved(int)));
	connect(signalSource(), SIGNAL(itemRemoved(int)), this, SIGNAL(sampleRemoved(int)));
	connect(signalSource(), SIGNAL(itemChanged(int)), this, SIGNAL(sampleChanged(int)));
}

QDateTime AMSamplePlate::dateTime() const{
	return dateTime_;
}

bool AMSamplePlate::addSample(AMSample *sample){
	bool retVal = append(sample, sample->name());
	if(retVal)
		connect(sample, SIGNAL(nameChanged(QString)), this, SLOT(onSampleNameChanged(QString)));
	return retVal;
}

void AMSamplePlate::onSampleNameChanged(const QString &name){
	qDebug() << "Just heard a name change to " << name;
	AMSample *sample = qobject_cast<AMSample*>(QObject::sender());
	if(sample){
		qDebug() << "It's a sample, so let's emit sampleChanged";
		emit sampleChanged(indexOfValue(sample));
	}
}

void AMSamplePlate::dbLoadDateTime(const QDateTime &newDateTime){
	dateTime_ = newDateTime;
}

AMDbObjectList AMSamplePlate::dbGetSamples(){
	AMDbObjectList retVal;
	for(int x = 0; x < count(); x++)
		retVal << this->operator[](x);
	return retVal;
}

void AMSamplePlate::dbLoadSamples(const AMDbObjectList &newSamples){
	clear();

	for(int x = 0; x < newSamples.count(); x++){
		AMSample *newSample = qobject_cast<AMSample*>(newSamples.at(x));
		if(newSample)
			append(newSample, newSample->name());
	}
}

