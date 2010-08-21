#include "AMSamplePlate.h"

AMSamplePlate::AMSamplePlate(QObject *parent) :
	QObject(parent)
{
	userName_ = "SGM";
	timeName_ = "loaded "+QDateTime::currentDateTime().toString("MMM d yyyy, h:m ap");
}

QString AMSamplePlate::plateName() const{
	return userName_+" "+timeName_;
}

int AMSamplePlate::count(){
	return samples_.count();
}

AMSamplePosition* AMSamplePlate::samplePositionAt(int index){
	if(index >= count())
		return NULL;
	return samples_.value(index);
}

AMSamplePosition* AMSamplePlate::samplePositionByName(const QString &name){
	if(sampleName2samplePosition_.containsF(name))
		return sampleName2samplePosition_.valueF(name);
	return NULL;
}

AMSample* AMSamplePlate::sampleAt(int index){
	if(index >= count())
		return NULL;
	return samplePositionAt(index)->sample();
}

AMSample* AMSamplePlate::sampleByName(const QString &name){
	if(sampleName2samplePosition_.containsF(name))
		return samplePositionByName(name)->sample();
	return NULL;
}

AMControlSetInfo* AMSamplePlate::positionAt(int index){
	if(index >= count())
		return NULL;
	return samplePositionAt(index)->position();
}

AMControlSetInfo* AMSamplePlate::positionByName(const QString &name){
	if(sampleName2samplePosition_.containsF(name))
		return samplePositionByName(name)->position();
	return NULL;
}

int AMSamplePlate::indexOf(const QString &name){
	if(sampleName2samplePosition_.containsF(name))
		return samples_.indexOf(samplePositionByName(name));
	return -1;
}

void AMSamplePlate::setName(const QString &name){
	userName_ = name;
}

bool AMSamplePlate::addSamplePosition(int index, AMSamplePosition *sp){
	if(index > count())
		return false;
	else if(!sp || !sp->sample() || !sp->position())
		return false;
	else if(samples_.contains(sp))
		return false;
	samples_.insert(index, sp);
	sampleName2samplePosition_.set(sp->sample()->name(), sp);
	return true;
}

bool AMSamplePlate::addSamplePosition(int index, AMSample *sample, AMControlSetInfo *position){
	if(index > count())
		return false;
	else if(!sample || !position)
		return false;
	AMSamplePosition *tmpSP = new AMSamplePosition(sample, position, this);
	return addSamplePosition(index, tmpSP);
}

bool AMSamplePlate::appendSamplePosition(AMSamplePosition *sp){
	return addSamplePosition(count(), sp);
}

bool AMSamplePlate::appendSamplePosition(AMSample *sample, AMControlSetInfo *position){
	return addSamplePosition(count(), sample, position);
}

bool AMSamplePlate::removeSamplePosition(AMSamplePosition *sp){
	return removeSamplePosition(samples_.indexOf(sp));
}

bool AMSamplePlate::removeSamplePosition(int index){
	if(index >= count())
		return false;
	AMSamplePosition *rSP = samples_.takeAt(index);
	sampleName2samplePosition_.removeR(rSP);
	return true;
}

AMSamplePosition::AMSamplePosition(AMSample *sample, AMControlSetInfo *position, QObject *parent) :
		QObject(parent)
{
	sample_ = sample;
	position_ = position;
	if(position_)
		connect(position_, SIGNAL(valuesChanged()), this, SIGNAL(positionValuesChanged()));
}

AMSample* AMSamplePosition::sample(){
	return sample_;
}

AMControlSetInfo* AMSamplePosition::position(){
	return position_;
}

void AMSamplePosition::setSample(AMSample *sample){
	sample_ = sample;
}

void AMSamplePosition::setPosition(AMControlSetInfo *position){
	if(position_)
		disconnect(position_, SIGNAL(valuesChanged()), this, SIGNAL(positionValuesChanged()));
	position_ = position;
	if(position_)
		connect(position_, SIGNAL(valuesChanged()), this, SIGNAL(positionValuesChanged()));
}
