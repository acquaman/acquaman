#include "AMDetectorInfo.h"

 AMDetectorInfo::~AMDetectorInfo(){}
AMDetectorInfo::AMDetectorInfo(const QString &name, const QString &description, const QString &units, double acquisitionTime, AMDetectorDefinitions::ReadMode readMode, QObject *parent) :
	AMDbObject(parent)
{
	setName(name);
	description_ = description;
	units_ = units;
	acquisitionTime_ = acquisitionTime;
	readMode_ = readMode;
}

void AMDetectorInfo::setValuesFrom(const AMDetectorInfo& other) {
	description_ = other.description();
	units_ = other.units();
	acquisitionTime_ = other.acquisitionTime();
	readMode_ = other.readMode();
	setName(other.name());
	setModified(true);
}

void AMDetectorInfo::setDescription(const QString &description){
	if(description_ != description){
		description_ = description;
		setModified(true);
	}
}

void AMDetectorInfo::setUnits(const QString &units){
	if(units_ != units){
		units_ = units;
		setModified(true);
	}
}

void AMDetectorInfo::setAcquisitionTime(double acquisitionTime){
	if(acquisitionTime_ != acquisitionTime){
		acquisitionTime_ = acquisitionTime;
		setModified(true);
	}
}

void AMDetectorInfo::setReadMode(AMDetectorDefinitions::ReadMode readMode){
	if(readMode_ != readMode){
		readMode_ = readMode;
		setModified(true);
	}
}
