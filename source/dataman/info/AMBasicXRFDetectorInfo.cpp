#include "AMBasicXRFDetectorInfo.h"

AMBasicXRFDetectorInfo::AMBasicXRFDetectorInfo(const QString &name, const QString &description, QObject *parent) :
	AMDetectorInfo(name, description, parent)
{
	setUnits("Counts");
}


AMBasicXRFDetectorInfo::AMBasicXRFDetectorInfo(const AMBasicXRFDetectorInfo &original) :
	AMDetectorInfo(original)
{
	this->operator =(original);
}

AMBasicXRFDetectorInfo& AMBasicXRFDetectorInfo::operator =(const AMBasicXRFDetectorInfo &other)
{
	if(this != &other){
		AMDetectorInfo::operator =(other);
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
	//ai.increment = AMNumber(scale());
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
	setChannels(num);
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
