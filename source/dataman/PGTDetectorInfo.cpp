#include "PGTDetectorInfo.h"

PGTDetectorInfo::PGTDetectorInfo(const QString& name, const QString& description, QObject *parent) : AMSpectralOutputDetectorInfo(name, description, 1024, "energy", QStringList(), parent)
{
	hvSetpoint_ = double(0.0);
	hvSetpointRangeMin_ = double(0.0);
	hvSetpointRangeMax_ = double(180.0);
}

PGTDetectorInfo::PGTDetectorInfo(const PGTDetectorInfo &original) :
		AMSpectralOutputDetectorInfo(original)
{
	retreiveAndSetProperties(original);
}


double PGTDetectorInfo::hvSetpoint() const {
	return hvSetpoint_;
}

double PGTDetectorInfo::hvSetpointRangeMin() const {
	return hvSetpointRangeMin_;
}

double PGTDetectorInfo::hvSetpointRangeMax() const {
	return hvSetpointRangeMax_;
}

QPair<double, double> PGTDetectorInfo::hvSetpointRange() const {
	return QPair<double, double>(hvSetpointRangeMin_, hvSetpointRangeMax_);
}

bool PGTDetectorInfo::hasDetails() const {
	return true;
}

void PGTDetectorInfo::setHVSetpoint(double hvSetpoint) {
	hvSetpoint_ = hvSetpoint;
	setModified(true);
}

void PGTDetectorInfo::setHVSetpointRangeMin(double min) {
	hvSetpointRangeMin_ = min;
	setModified(true);
}

void PGTDetectorInfo::setHVSetpointRangeMax(double max) {
	hvSetpointRangeMax_ = max;
	setModified(true);
}

void PGTDetectorInfo::setHVSetpointRange(QPair<double, double> range){
	hvSetpointRangeMin_ = range.first;
	hvSetpointRangeMax_ = range.second;
	setModified(true);
}
