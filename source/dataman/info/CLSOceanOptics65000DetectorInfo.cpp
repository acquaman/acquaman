/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "CLSOceanOptics65000DetectorInfo.h"

 CLSOceanOptics65000DetectorInfo::~CLSOceanOptics65000DetectorInfo(){}
CLSOceanOptics65000DetectorInfo::CLSOceanOptics65000DetectorInfo(const QString &name, const QString &description, QObject *parent) :
		AMOldDetectorInfo(name, description, parent)
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

CLSOceanOptics65000DetectorInfo::CLSOceanOptics65000DetectorInfo(const CLSOceanOptics65000DetectorInfo &original) :
		AMOldDetectorInfo(original)
{
//	retreiveAndSetProperties(original);
	this->operator =(original);
}

AMOldDetectorInfo* CLSOceanOptics65000DetectorInfo::toNewInfo() const{
	return new CLSOceanOptics65000DetectorInfo(*this);
}

CLSOceanOptics65000DetectorInfo& CLSOceanOptics65000DetectorInfo::operator =(const CLSOceanOptics65000DetectorInfo& other){
	if(this != &other){
//		retreiveAndSetProperties(other);
		AMOldDetectorInfo::operator =(other);
		setBinCount(other.binCount());
		setAxisName(other.axisName());
		setAxisUnit(other.axisUnit());
		setIntegrationTime(other.integrationTime());
		setUnits(other.units());
	}
	return *this;
}

int CLSOceanOptics65000DetectorInfo::binCount() const{
	return binCount_;
}

QString CLSOceanOptics65000DetectorInfo::axisName() const{
	return axisName_;
}

QString CLSOceanOptics65000DetectorInfo::axisUnit() const{
	return axisUnit_;
}

double CLSOceanOptics65000DetectorInfo::integrationTime() const{
	return integrationTime_;
}

int CLSOceanOptics65000DetectorInfo::rank() const{
	return 1;
}

AMnDIndex CLSOceanOptics65000DetectorInfo::size() const{
	return AMnDIndex(binCount());
}

QList<AMAxisInfo> CLSOceanOptics65000DetectorInfo::axes() const{
	QList<AMAxisInfo> rv;
	AMAxisInfo binAxis(name()+"_x", binCount(), description()+" axis", units());
	rv << binAxis;
	return rv;
}

QDebug CLSOceanOptics65000DetectorInfo::qDebugPrint(QDebug &d) const{
	d << name() << integrationTime() << "[integrationTime]";
	return d;
}

bool CLSOceanOptics65000DetectorInfo::hasDetails() const{
	return true;
}

void CLSOceanOptics65000DetectorInfo::setIntegrationTime(double integrationTime){
	if(integrationTime_ != integrationTime){
		integrationTime_ = integrationTime;
		setModified(true);
	}
}

void CLSOceanOptics65000DetectorInfo::setBinCount(int binCount){
	if(binCount_ != binCount){
		binCount_ = binCount;
		setModified(true);
	}
}

void CLSOceanOptics65000DetectorInfo::setAxisName(const QString &axisName){
	if(axisName_ != axisName){
		axisName_ = axisName;
		setModified(true);
	}
}

void CLSOceanOptics65000DetectorInfo::setAxisUnit(const QString &axisUnit){
	if(axisUnit_ != axisUnit){
		axisUnit_ = axisUnit;
		setModified(true);
	}
}
