#include "Control.h"

ReadOnlyPVControl::ReadOnlyPVControl(const QString& name, const QString& readPVname, const QString& movingPVname, QObject* parent) : Control(name, "?", parent), wasMoving_(false) {

	readPV_ = new DoubleProcessVariable(readPVname, true, this);
	// TODO: check if this failed to connect channel. Also, handle disconnects/breakdowns.
	connect(readPV_, SIGNAL(valueChanged(double)), this, SIGNAL(valueChanged(double)));
	connect(readPV_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(readPV_, SIGNAL(error(int)), this, SLOT(onPVError(int)));
	connect(readPV_, SIGNAL(connectionTimeout()), this, SLOT(onConnectionTimeout()));

//	StringProcessVariable* readPVunits = new StringProcessVariable(readPVname+".EGU", false, this);
//	connect(readPVunits, SIGNAL(valueChanged(const QString&)), this, SLOT(onUnitsReporting(const QString&)));
	connect(readPV_, SIGNAL(initialized()), this, SLOT(onReadPVInitialized()));

	if(movingPVname != "") {
		movingPV_ = new IntProcessVariable(movingPVname, true, this);
		connect(movingPV_, SIGNAL(valueChanged(int)), this, SLOT(onMovingChanged(int)));
	}
	else
		movingPV_ = 0;

}

// This is written out fully (rather than simply connected SIGNALs to SIGNALs
// So that we can override it in ReadWritePVControl.
void ReadOnlyPVControl::onPVConnected(bool) {

	// For the read-only control, just having the readPV is enough to be connected or disconnected.
	emit connected( isConnected() );

	if(isConnected()) {
		setState(Control::CanMeasure);
	}
	else {
		setState(Control::NotConnected);
	}
}

void ReadOnlyPVControl::onPVError(int error) {
	// TODO: figure out how to handle this best.
	qDebug() << QString("Process Variable error %1: %2.").arg(readPV_->pvName()).arg(error);
}

void ReadOnlyPVControl::onMovingChanged(int isMoving) {
	emit moving(isMoving);

	if(wasMoving_ && !isMoving)
		onMoveEnded();
	if(!wasMoving_ && isMoving)
		onMoveStarted();

	wasMoving_ = isMoving;
}

void ReadOnlyPVControl::onReadPVInitialized() {
	setUnits(readPV_->units());	// copy over the new unit string
	qDebug() << QString("ReadOnlyPVControl: read PV initialized correctly.");
}

PVControl::PVControl(const QString& name, const QString& readPVname, const QString& writePVname, const QString& movingPVname, double tolerance, QObject* parent) : ReadOnlyPVControl(name, readPVname, movingPVname, parent) {

	tolerance_ = tolerance;

	writePV_ = new DoubleProcessVariable(writePVname, true, this);
	// TODO: check if this failed to connect channel. Also, handle disconnects/breakdowns.
	// connect(writePV_, SIGNAL(valueChanged(double)), this, SIGNAL(valueChanged(double)));
	connect(writePV_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(writePV_, SIGNAL(error(int)), this, SLOT(onPVError(int)));
	connect(writePV_, SIGNAL(connectionTimeout()), this, SIGNAL(writeConnectionTimeout()));
	connect(writePV_, SIGNAL(connectionTimeout()), this, SLOT(onConnectionTimeout()));

	// TODO: actually moving; write failed-->moveFailed; sort out signals.
	// TODO: decide if monitoring the set value is desireable (notif. that you exceeded limits?)
}

void PVControl::move(double value) {

	//onMoveStarted(); TODO: flag this manually if we have no connection from the movingPV_?

	moveTarget_ = value;
	// QString stringVersion = QString("%1").arg(moveTarget_, -1, 'e', 4);

	if( canMove() ) {
		writePV_->setValue(moveTarget_);
		qDebug() << QString("Moving %1 to %2").arg(writePV_->pvName()).arg(moveTarget_);
	}
	else {
		qDebug() << QString("Could not move %1 to %2").arg(writePV_->pvName()).arg(value);
		onMoveFailed();
	}


}

void PVControl::stop() {

}

void PVControl::onConnectionTimeout() {
	if(sender() == readPV_) {
		emit readConnectionTimeout();
		setState( state() & ~Control::CanMeasure );	// Disable CanMeasure flag
		emit connected(false);						// Not having a readPV_ makes us disconnected.
	}
	if(sender() == writePV_) {
		emit writeConnectionTimeout();
		setState( state() & ~Control::CanMove );	// disable CanMove flag
		emit connected(false);						// Not having a writePV_ makes us disconnected.
	}
}

void PVControl::onPVConnected(bool) {
	// This will happen if either the readPV_ or writePV_ connects, or disconnects. That makes lots of cases.
	// We can distinguish based on sender() == readPV_ or sender() == writePV_. But we don't have to.

	// Let's just check using our isConnected() function, which guarantees that we can read from readPV_ and write to writePV_.
	// It will re-check for connection status on these two PVs.
	emit connected( this->isConnected() );

	int state = Control::NotConnected;
	if( readPV_->canRead() )
		state |= Control::CanMeasure;
	if (writePV_->canWrite() )
		state |= Control::CanMove;

	setState(state);
}

void PVControl::onPVError(int error) {
	// TODO
	if(sender() == readPV_)
		qDebug() << QString("ProcessVariable error %1: %2.").arg(readPV_->pvName()).arg(error);
	if(sender() == writePV_)
		qDebug() << QString("ProcessVariable error %1: %2.").arg(writePV_->pvName()).arg(error);
}
