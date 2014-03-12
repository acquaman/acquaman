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


#include "AMSpectralOutputDetectorInfo.h"

 AMSpectralOutputDetectorInfo::~AMSpectralOutputDetectorInfo(){}
AMSpectralOutputDetectorInfo::AMSpectralOutputDetectorInfo(const QString& name, const QString& description, int binCount, QString axisName, QStringList binNames, QObject *parent) :
		AMOldDetectorInfo(name, description, parent)
{
	binCount_ = binCount;
	axisName_ = axisName;
	binNames_ = binNames;
	integrationTime_ = double(1.0);
	integrationTimeRangeMin_ = double(0.0);
	integrationTimeRangeMax_ = double(10.0);
	integrationModeList_ << "Real" << "Live" << "Peak";
	integrationMode_ = integrationModeList_.at(0);
	units_ = "counts";
}

AMSpectralOutputDetectorInfo::AMSpectralOutputDetectorInfo(const AMSpectralOutputDetectorInfo &original) :
		AMOldDetectorInfo(original)
{
//	retreiveAndSetProperties(original);
	integrationModeList_ << "Real" << "Live" << "Peak";
	this->operator =(original);
}

AMSpectralOutputDetectorInfo& AMSpectralOutputDetectorInfo::operator =(const AMSpectralOutputDetectorInfo &other)
{
	if(this != &other){
		AMOldDetectorInfo::operator =(other);
		setBinCount(other.binCount());
		setAxisName(other.axisName());
		setBinNames(other.binNames());
		setIntegrationTime(other.integrationTime());
		setIntegrationTimeRangeMin(other.integrationTimeRangeMin());
		setIntegrationTimeRangeMax(other.integrationTimeRangeMax());
		setIntegrationMode(other.integrationMode());
		setUnits(other.units());
	}
	return *this;
}

int AMSpectralOutputDetectorInfo::binCount() const
{
	return binCount_;
}

QString AMSpectralOutputDetectorInfo::axisName() const
{
	return axisName_;
}

QStringList AMSpectralOutputDetectorInfo::binNames() const
{
	return binNames_;
}

double AMSpectralOutputDetectorInfo::integrationTime() const
{
	return integrationTime_;
}

AMRange AMSpectralOutputDetectorInfo::integrationTimeRange() const
{
	return AMRange(integrationTimeRangeMin_, integrationTimeRangeMax_);
}

double AMSpectralOutputDetectorInfo::integrationTimeRangeMin() const
{
	return integrationTimeRangeMin_;
}

double AMSpectralOutputDetectorInfo::integrationTimeRangeMax() const
{
	return integrationTimeRangeMax_;
}

QString AMSpectralOutputDetectorInfo::integrationMode() const
{
	return integrationMode_;
}

QStringList AMSpectralOutputDetectorInfo::integrationModeList() const
{
	return integrationModeList_;
}


int AMSpectralOutputDetectorInfo::rank() const
{
	return 1;
}

AMnDIndex AMSpectralOutputDetectorInfo::size() const
{
	return AMnDIndex(binCount());
}

QList<AMAxisInfo> AMSpectralOutputDetectorInfo::axes() const
{
	QList<AMAxisInfo> rv;
	AMAxisInfo binAxis(name()+"_x", binCount(), description() + " axis" );	/// \todo better name description, and provide units
	rv << binAxis;
	return rv;
}

void AMSpectralOutputDetectorInfo::setBinCount(int numBins)
{
	binCount_ = numBins;
	setModified(true);
}

void AMSpectralOutputDetectorInfo::setAxisName(const QString& axisName)
{
	axisName_ = axisName;
	setModified(true);
}

void AMSpectralOutputDetectorInfo::setBinNames(const QStringList& binNames)
{
	binNames_ = binNames;
	setModified(true);
}

void AMSpectralOutputDetectorInfo::setIntegrationTime(double integrationTime)
{
	integrationTime_ = integrationTime;
	setModified(true);
}

void AMSpectralOutputDetectorInfo::setIntegrationTimeRangeMin(double min)
{
	integrationTimeRangeMin_ = min;
	setModified(true);
}

void AMSpectralOutputDetectorInfo::setIntegrationTimeRangeMax(double max)
{
	integrationTimeRangeMax_ = max;
	setModified(true);
}

void AMSpectralOutputDetectorInfo::setIntegrationTimeRange(const AMRange &range)
{
	integrationTimeRangeMin_ = range.minimum();
	integrationTimeRangeMax_ = range.maximum();
	setModified(true);
}

void AMSpectralOutputDetectorInfo::setIntegrationMode(const QString& integrationMode)
{
	/// \todo check that integrationMode is one of valid choices.
	integrationMode_ = integrationMode;
	setModified(true);
}
