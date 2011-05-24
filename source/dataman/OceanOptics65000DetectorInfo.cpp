#include "OceanOptics65000DetectorInfo.h"

OceanOptics65000DetectorInfo::OceanOptics65000DetectorInfo(const QString &name, const QString &description, QObject *parent) :
		AMDetectorInfo(name, description, parent)
{
	binCount_ = 0;
	axisName_ = "";
	axisUnit_ = "";
	integrationTime_ = 0.0;
	units_ = "";
	setBinCount(1024);
	setAxisName("Wavelength");
	setAxisUnit("nm");
	setIntegrationTime(1.0);
	setUnits("counts");
}

OceanOptics65000DetectorInfo::OceanOptics65000DetectorInfo(const OceanOptics65000DetectorInfo &original)
{
	retreiveAndSetProperties(original);
}

AMDetectorInfo* OceanOptics65000DetectorInfo::toNewInfo() const{
	return new OceanOptics65000DetectorInfo(*this);
}

OceanOptics65000DetectorInfo& OceanOptics65000DetectorInfo::operator =(const OceanOptics65000DetectorInfo& other){
	if(this != &other)
		retreiveAndSetProperties(other);
	return *this;
}

int OceanOptics65000DetectorInfo::binCount() const{
	return binCount_;
}

QString OceanOptics65000DetectorInfo::axisName() const{
	return axisName_;
}

QString OceanOptics65000DetectorInfo::axisUnit() const{
	return axisUnit_;
}

double OceanOptics65000DetectorInfo::integrationTime() const{
	return integrationTime_;
}

int OceanOptics65000DetectorInfo::rank() const{
	return 1;
}

AMnDIndex OceanOptics65000DetectorInfo::size() const{
	return AMnDIndex(binCount());
}

QList<AMAxisInfo> OceanOptics65000DetectorInfo::axes() const{
	QList<AMAxisInfo> rv;
	AMAxisInfo binAxis(name()+"_x", binCount(), description()+" axis", units());
	rv << binAxis;
	return rv;
}

QDebug OceanOptics65000DetectorInfo::qDebugPrint(QDebug &d) const{
	d << integrationTime() << "[integrationTime]";
	return d;
}

bool OceanOptics65000DetectorInfo::hasDetails() const{
	return true;
}

void OceanOptics65000DetectorInfo::setIntegrationTime(double integrationTime){
	if(integrationTime_ != integrationTime){
		integrationTime_ = integrationTime;
		setModified(true);
	}
}

void OceanOptics65000DetectorInfo::setBinCount(int binCount){
	if(binCount_ != binCount){
		binCount_ = binCount;
		setModified(true);
	}
}

void OceanOptics65000DetectorInfo::setAxisName(const QString &axisName){
	if(axisName_ != axisName){
		axisName_ = axisName;
		setModified(true);
	}
}

void OceanOptics65000DetectorInfo::setAxisUnit(const QString &axisUnit){
	if(axisUnit_ != axisUnit){
		axisUnit_ = axisUnit;
		setModified(true);
	}
}
