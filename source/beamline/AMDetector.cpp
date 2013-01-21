#include "AMDetector.h"

AMDetector::AMDetector(const QString &name, const QString &description, QObject *parent) :
	QObject(parent)
{
	setObjectName(name);
	description_ = description;

	connected_ = false;
	readyForAcquisition_ = false;
	powered_ = false;
}

AMDetectorInfo AMDetector::toInfo() const{
	return AMDetectorInfo(name(), description(), units(), acquisitionTime(), readMode());
}

AMDetector::operator AMMeasurementInfo() {
	// This is code included from the previous AMDetector(Info) system. This may be important for the transition. [DKC January 14th, 2013]
	//return AMMeasurementInfo(name(), description(), units(), axes());
	if(!description().isEmpty())
		return AMMeasurementInfo(description().remove(" "), description(), units(), axes());
	else
		return AMMeasurementInfo(name(), name(), units(), axes());
}

bool AMDetector::reading0D(const AMnDIndex &startIndex, const AMnDIndex &endIndex, double *outputValues) const{
	if(!checkValid(startIndex, endIndex))
		return false;

	if(startIndex != endIndex)
		return false;

	AMNumber retVal = reading(startIndex);
	if(!retVal.isValid())
		return false;

	*outputValues = double(retVal);
	return true;
}

bool AMDetector::reading1D(const AMnDIndex &startIndex, const AMnDIndex &endIndex, double *outputValues) const{
	if(!checkValid(startIndex, endIndex))
		return false;

	return true;
}

bool AMDetector::reading2D(const AMnDIndex &startIndex, const AMnDIndex &endIndex, double *outputValues) const{
	if(!checkValid(startIndex, endIndex))
		return false;

	return true;
}

bool AMDetector::readingND(int dimensionality, const AMnDIndex &startIndex, const AMnDIndex &endIndex, double *outputValues) const{
	switch(dimensionality) {
	case 0:
		return reading0D(startIndex, endIndex, outputValues);
	case 1:
		return reading1D(startIndex, endIndex, outputValues);
	case 2:
		return reading2D(startIndex, endIndex, outputValues);
	default:
		return false;
	}
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

void AMDetector::setPowered(bool isPowered){
	if(isPowered != powered_){
		powered_ = isPowered;
		emit powered(powered_);
	}
}

bool AMDetector::checkValid(const AMnDIndex &startIndex, const AMnDIndex &endIndex) const{
	int detectorRank = rank();

	if(startIndex.rank() != detectorRank || endIndex.rank() != detectorRank || startIndex.rank() != endIndex.rank())
		return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
	for(int mu=0; mu<detectorRank; ++mu) {
		if(endIndex.at(mu) >= size(mu))
			return false;
		if(endIndex.at(mu) < startIndex.at(mu))
			return false;
	}
#endif
	return true;
}
