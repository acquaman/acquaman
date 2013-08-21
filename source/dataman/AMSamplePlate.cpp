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

