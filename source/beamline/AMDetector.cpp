#include "AMDetector.h"

AMDetector::AMDetector(const QString &name, const QString &description, QObject *parent) :
	QObject(parent)
{
	setObjectName(name);
	description_ = description;

	connected_ = false;
	readyForAcquisition_ = false;
}

AMDetector::operator AMMeasurementInfo() {
	// This is code included from the previous AMDetector(Info) system. This may be important for the transition. [DKC January 14th, 2013]
	//return AMMeasurementInfo(name(), description(), units(), axes());
	if(!description().isEmpty())
		return AMMeasurementInfo(description().remove(" "), description(), units(), axes());
	else
		return AMMeasurementInfo(name(), name(), units(), axes());
}

void AMDetector::setConnected(bool isConnected){
	if(isConnected != connected_){
		connected_ = isConnected;
		emit connected(connected_);
	}
}

void AMDetector::setReadyForAcquisition(bool isReadyForAcquisition){
	if(isReadyForAcquisition != readyForAcquisition_){
		readyForAcquisition_ = isReadyForAcquisition;
		emit readyForAcquisition(readyForAcquisition_);
	}
}
