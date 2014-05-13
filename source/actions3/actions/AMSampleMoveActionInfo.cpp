#include "AMSampleMoveActionInfo.h"

#include "dataman/AMSample.h"
#include "dataman/database/AMConstDbObject.h"

AMSampleMoveActionInfo::AMSampleMoveActionInfo(const AMSample *sample, QObject *parent) :
	AMActionInfo3("Sample Move", "Sample Move", ":/system-run.png", parent )
{
	sample_ = 0; //NULL
	setSample(sample);
}

AMSampleMoveActionInfo::AMSampleMoveActionInfo(const AMSampleMoveActionInfo &other) :
	AMActionInfo3(other)
{
	sample_ = 0; //NULL
	setSample(other.sample());
}

AMSampleMoveActionInfo::~AMSampleMoveActionInfo()
{
}

AMActionInfo3* AMSampleMoveActionInfo::createCopy() const{
	return new AMSampleMoveActionInfo(*this);
}

QString AMSampleMoveActionInfo::typeDescription() const{
	return "Sample Move";
}

const AMSample* AMSampleMoveActionInfo::sample() const{
	const AMSample *asSample = qobject_cast<const AMSample*>(sample_->object());
	return asSample;
}

void AMSampleMoveActionInfo::setSample(const AMSample *sample){
	if(!sample_)
		sample_ = new AMConstDbObject(sample);
	else
		sample_->setObject(sample);
	setModified(true);

	updateDescriptionText();
}

void AMSampleMoveActionInfo::updateDescriptionText(){
	QString description;
	if(sample_->object())
		description = QString("Moving to %1").arg(sample_->object()->name());
	else
		description = QString("No Sample Selected");

	setShortDescription(description);
	setLongDescription(description);
}

AMConstDbObject* AMSampleMoveActionInfo::dbReadSample() const{
	return sample_;
}

void AMSampleMoveActionInfo::dbLoadSample(AMConstDbObject *newSample){
	sample_ = newSample;
}
