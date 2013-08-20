#include "AMSamplePlate.h"

AMSamplePlate::AMSamplePlate(QObject *parent) :
	AMDbObject(parent), AMOrderedSet<QString, AMSample*>()
{
	dateTime_ = QDateTime::currentDateTime();
}

QDateTime AMSamplePlate::dateTime() const{
	return dateTime_;
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

