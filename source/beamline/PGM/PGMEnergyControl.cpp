#include "PGMEnergyControl.h"

PGMEnergyControl::PGMEnergyControl(const QString& name, const QString& readPVname, const QString& writePVname, const QString& movingPVname, const QString& stopPVname, QObject* parent, double tolerance, double moveStartTimeoutSeconds, AMAbstractControlStatusChecker* statusChecker, int stopValue, const QString &description)
	: AMPVwStatusControl(name, readPVname, writePVname, movingPVname, stopPVname, parent, tolerance, moveStartTimeoutSeconds, statusChecker, stopValue, description)
{

}

PGMEnergyControl::~PGMEnergyControl()
{

}

bool PGMEnergyControl::isConnected() const
{
	// We want the energy control to report as disconnected when:
	//		- the read PV can't be read.
	//		- the write PV isn't write ready, if it can be written to.
	//		- the stop PV can't be written to, if it is provided.
	//		- the moving status PV can't be read.

	bool readOK = ( readPV_->canRead() && readPV_->readReady() );
	bool writeOK = ( writePV_->canWrite() ? writePV_->writeReady() : true );
	bool stopOK = ( noStopPV_ ? true : (stopPV_->canWrite() && stopPV_->writeReady()) );
	bool statusOK = ( movingPV_->canRead() && movingPV_->readReady() );

	return ( readOK && writeOK && stopOK && statusOK );
}

bool PGMEnergyControl::canMeasure() const
{
	// We want the energy control to report as not measurable when:
	//		- the read PV can't be read.

	return ( readPV_->canRead() && readPV_->readReady() );
}

bool PGMEnergyControl::canMove() const
{
	// We want the energy control to report as not movable when:
	//		- the write PV can't be written to.

	return ( writePV_->canWrite() && writePV_->writeReady() );
}

