#include "PGMPicoAmmeter.h"

#include "beamline/PGM/PGMBeamline.h"

PGMPicoAmmeter::PGMPicoAmmeter(const QString &name, const QString &description, const QString &pvName, QObject *parent)
	: AMDetector(name, description, parent)
{
	control_ = new AMReadOnlyPVControl(name, pvName, this, description);
	dataSource_ = new AM0DProcessVariableDataSource(control_->readPV(), name, this);

	connect(control_->readPV(), SIGNAL(readReadyChanged(bool)), this, SLOT(updateUnits()));
	connect(control_, SIGNAL(valueChanged(double)), this, SIGNAL(valueChanged(double)));
}

double PGMPicoAmmeter::acquisitionTime() const
{
	return PGMBeamline::pgm()->synchronizedDwellTime()->time();
}

bool PGMPicoAmmeter::currentlySynchronizedDwell() const
{
	return true;
}

QString PGMPicoAmmeter::synchronizedDwellKey() const
{
	return "A1611I2:start_read NPP NMS";
}

AMDetectorTriggerSource *PGMPicoAmmeter::detectorTriggerSource()
{
	return PGMBeamline::pgm()->synchronizedDwellTime()->triggerSource();
}

AMDetectorDwellTimeSource *PGMPicoAmmeter::detectorDwellTimeSource()
{
	return PGMBeamline::pgm()->synchronizedDwellTime()->dwellTimeSource();
}

AMNumber PGMPicoAmmeter::reading(const AMnDIndex &indexes) const
{
	Q_UNUSED(indexes)
	return dataSource_->value(AMnDIndex());
}

AMNumber PGMPicoAmmeter::singleReading() const
{
	return dataSource_->value(AMnDIndex());
}

AMDataSource *PGMPicoAmmeter::dataSource() const
{
	return dataSource_;
}

bool PGMPicoAmmeter::setAcquisitionTime(double seconds)
{
	PGMBeamline::pgm()->synchronizedDwellTime()->setTime(seconds);
	return true;
}

void PGMPicoAmmeter::updateUnits()
{
	units_ = control_->readPV()->units();
}

bool PGMPicoAmmeter::initializeImplementation()
{
	setInitialized();
	return true;
}

bool PGMPicoAmmeter::acquireImplementation(AMDetectorDefinitions::ReadMode readMode)
{
	Q_UNUSED(readMode)
	setAcquiring();
	setAcquisitionSucceeded();
	setReadyForAcquisition();
	return true;
}

bool PGMPicoAmmeter::cleanupImplementation()
{
	setCleanedUp();
	return true;
}

bool PGMPicoAmmeter::data(double *outputValues) const
{
	outputValues[0] = singleReading();
	return true;
}

