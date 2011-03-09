#include "AMSpectralOutputDetectorInfo.h"

AMSpectralOutputDetectorInfo::AMSpectralOutputDetectorInfo(const QString& name, const QString& description, int binCount, QString axisName, QStringList binNames, QObject *parent) :
		AMDetectorInfo(name, description, parent)
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
		AMDetectorInfo(original)
{
	retreiveAndSetProperties(original);
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

QPair<double, double> AMSpectralOutputDetectorInfo::integrationTimeRange() const
{
	QPair<double, double> rVal;
	rVal.first = integrationTimeRangeMin_;
	rVal.second =integrationTimeRangeMax_;
	return rVal;
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

void AMSpectralOutputDetectorInfo::setIntegrationTimeRange(QPair<double, double> range)
{
	integrationTimeRangeMin_ = range.first;
	integrationTimeRangeMax_ = range.second;
	setModified(true);
}

void AMSpectralOutputDetectorInfo::setIntegrationMode(const QString& integrationMode)
{
	/// \todo check that integrationMode is one of valid choices.
	integrationMode_ = integrationMode;
	setModified(true);
}
