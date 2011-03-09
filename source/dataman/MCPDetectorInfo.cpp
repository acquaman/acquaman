#include "MCPDetectorInfo.h"

MCPDetectorInfo::MCPDetectorInfo(const QString& name, const QString& description, QObject *parent) : AMDetectorInfo(name, description, parent)
{
	hvSetpoint_ = double(0.0);
	hvSetpointRangeMin_ = double(0.0);
	hvSetpointRangeMax_ = double(1400.0);
	units_ = "counts";
}

MCPDetectorInfo::MCPDetectorInfo(const MCPDetectorInfo &original) :
		AMDetectorInfo(original)
{
	retreiveAndSetProperties(original);
}

double MCPDetectorInfo::hvSetpoint() const
{
	return hvSetpoint_;
}

double MCPDetectorInfo::hvSetpointRangeMin() const
{
	return hvSetpointRangeMin_;
}

double MCPDetectorInfo::hvSetpointRangeMax() const
{
	return hvSetpointRangeMax_;
}

QPair<double, double> MCPDetectorInfo::hvSetpointRange() const
{
	return QPair<double, double>(hvSetpointRangeMin_, hvSetpointRangeMax_);
}

bool MCPDetectorInfo::hasDetails() const
{
	return true;
}

void MCPDetectorInfo::setHVSetpoint(double hvSetpoint) {
	hvSetpoint_ = hvSetpoint;
	setModified(true);
}

void MCPDetectorInfo::setHVSetpointRangeMin(double min) {
	hvSetpointRangeMin_ = min;
	setModified(true);
}

void MCPDetectorInfo::setHVSetpointRangeMax(double max) {
	hvSetpointRangeMax_ = max;
	setModified(true);
}

void MCPDetectorInfo::setHVSetpointRange(QPair<double, double> range){
	hvSetpointRangeMin_ = range.first;
	hvSetpointRangeMax_ = range.second;
	setModified(true);
}
