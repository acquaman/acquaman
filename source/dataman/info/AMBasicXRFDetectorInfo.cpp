/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#include "AMBasicXRFDetectorInfo.h"

 AMBasicXRFDetectorInfo::~AMBasicXRFDetectorInfo(){}
AMBasicXRFDetectorInfo::AMBasicXRFDetectorInfo(const QString &name, const QString &description, QObject *parent) :
	AMOldDetectorInfo(name, description, parent)
{
	setUnits("Counts");
}


AMBasicXRFDetectorInfo::AMBasicXRFDetectorInfo(const AMBasicXRFDetectorInfo &original) :
	AMOldDetectorInfo(original)
{
	this->operator =(original);
}

AMBasicXRFDetectorInfo& AMBasicXRFDetectorInfo::operator =(const AMBasicXRFDetectorInfo &other)
{
	if(this != &other){
		AMOldDetectorInfo::operator =(other);
		setElements(other.elements());
		setChannels(other.channels());
		setIntegrationTime(other.integrationTime());
		setPeakingTime(other.peakingTime());
	}
	return *this;
}

int AMBasicXRFDetectorInfo::rank() const
{
	return 1;
}
AMnDIndex AMBasicXRFDetectorInfo::size() const
{
	return AMnDIndex(channels_);
}

QList<AMAxisInfo> AMBasicXRFDetectorInfo::axes() const
{
	QList<AMAxisInfo> axisInfo;
	AMAxisInfo ai("Energy", channels(), "Energy", "eV");
	ai.start = AMNumber(0);
	ai.isUniform = true;
	axisInfo << ai;
	return axisInfo;
}

int AMBasicXRFDetectorInfo::channels() const
{
	return channels_;
}

int AMBasicXRFDetectorInfo::elements() const
{
	return elements_;
}

double AMBasicXRFDetectorInfo::integrationTime() const
{
	return integrationTime_;
}

double AMBasicXRFDetectorInfo::peakingTime() const
{
	return peakingTime_;
}

bool AMBasicXRFDetectorInfo::setSize(const AMnDIndex &size)
{
	if (size.rank() != 1)
		return false;

	channels_ = size.i();
	setModified(true);
	return true;
}

void AMBasicXRFDetectorInfo::setChannels(int num)
{
	channels_ = num;
	setModified(true);
}

void AMBasicXRFDetectorInfo::setChannelsAsDouble(double num){
	setChannels(int(num));
}

void AMBasicXRFDetectorInfo::setElements(int num)
{
	elements_ = num;

	activeElements_.clear();
	for (int i = 0; i < num; i++)
		activeElements_ << true;

	setModified(true);
}

void AMBasicXRFDetectorInfo::setIntegrationTime(double time)
{
	integrationTime_ = time;
	setModified(true);
}

void AMBasicXRFDetectorInfo::setPeakingTime(double time)
{
	peakingTime_ = time;
	setModified(true);
}
