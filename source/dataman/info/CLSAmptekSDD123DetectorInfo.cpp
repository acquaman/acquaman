#include "CLSAmptekSDD123DetectorInfo.h"

CLSAmptekSDD123DetectorInfo::CLSAmptekSDD123DetectorInfo(const QString &name, const QString &description, QObject *parent) :
	AMBasicXRFDetectorInfo(name, description, parent)
{
	setUnits("Counts");
}

CLSAmptekSDD123DetectorInfo::CLSAmptekSDD123DetectorInfo(const CLSAmptekSDD123DetectorInfo &original) :
	AMBasicXRFDetectorInfo(original)
{
//	retreiveAndSetProperties(original);
	this->operator =(original);
}

AMDetectorInfo* CLSAmptekSDD123DetectorInfo::toNewInfo() const{
	return new CLSAmptekSDD123DetectorInfo(*this);
}

CLSAmptekSDD123DetectorInfo& CLSAmptekSDD123DetectorInfo::operator =(const CLSAmptekSDD123DetectorInfo &other)
{
	if(this != &other){
//		retreiveAndSetProperties(other);
		AMBasicXRFDetectorInfo::operator =(other);
		setUnits(other.units());
		setDetectorTemperature(other.detectorTemperature());
	}
	return *this;
}

double CLSAmptekSDD123DetectorInfo::detectorTemperature() const
{
	return detectorTemperature_;
}

void CLSAmptekSDD123DetectorInfo::setDetectorTemperature(double detectorTemperature)
{
	if(detectorTemperature_ != detectorTemperature){
		detectorTemperature_ = detectorTemperature;
		setModified(true);
	}
}
