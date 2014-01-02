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


#include "CLSBiStateControl.h"
#include "util/AMErrorMonitor.h"

 CLSBiStateControl::~CLSBiStateControl(){}
CLSBiStateControl::CLSBiStateControl(const QString &name, const QString &description, const QString &state, const QString &open, const QString &close, AMAbstractControlStatusChecker *statusChecker, QObject *parent)
	: AMControl(name, "", parent, description)
{
	isConnected_ = false;
	isMoving_ = false;
	moveInProgress_ = false;
	setTolerance(0.1);

	statusChecker_ = statusChecker;
	setpoint_ = 2;

	statePV_ = new AMProcessVariable(state, true, this);
	openPV_ = new AMProcessVariable(open, true, this);	// should these actually be monitoring? Necessary?
	closePV_ = new AMProcessVariable(close, true, this); // should these actually be monitoring? Necessary?

	connect(statePV_, SIGNAL(valueChanged(int)), this, SLOT(onStateChanged()));
	connect(statePV_, SIGNAL(alarmChanged(int,int)), this, SIGNAL(alarmChanged(int,int)));

	// connection state monitoring
	connect(statePV_, SIGNAL(connected(bool)), this, SLOT(onConnectionStateChanged()));
	connect(openPV_, SIGNAL(connected(bool)), this, SLOT(onConnectionStateChanged()));
	connect(closePV_, SIGNAL(connected(bool)), this, SLOT(onConnectionStateChanged()));
	connect(statePV_, SIGNAL(initialized()), this, SLOT(onConnectionStateChanged()));
	connect(openPV_, SIGNAL(initialized()), this, SLOT(onConnectionStateChanged()));
	connect(closePV_, SIGNAL(initialized()), this, SLOT(onConnectionStateChanged()));
	connect(statePV_, SIGNAL(hasValuesChanged(bool)), this, SLOT(onConnectionStateChanged()));
	connect(openPV_, SIGNAL(hasValuesChanged(bool)), this, SLOT(onConnectionStateChanged()));
	connect(closePV_, SIGNAL(hasValuesChanged(bool)), this, SLOT(onConnectionStateChanged()));
}

void CLSBiStateControl::onConnectionStateChanged()
{
	bool wasConnected = isConnected_;
	isConnected_ = isConnected();
	if(wasConnected != isConnected_)
		emit connected(isConnected_);
}

void CLSBiStateControl::onStateChanged()
{
	emit valueChanged(state());
	emit stateChanged(state());

	if(moveInProgress_) {
		if(inPosition()) {
			moveInProgress_ = false;
			emit moveSucceeded();
		}
	}

	bool wasMoving = isMoving_;
	isMoving_ = isMoving();
	if(isMoving_ != wasMoving)
		emit movingChanged(isMoving_);
}

AMControl::FailureExplanation CLSBiStateControl::open() {
	// already transitioning? Cannot send while moving.

	if(isMoving() && !allowsMovesWhileMoving()) {
		AMErrorMon::alert(this, -1, QString("Cannot open %1: it's currently in the process of opening or closing.").arg(name()));
		return AlreadyMovingFailure;
	}
	setpoint_ = 1;
	if(!moveInProgress_)
		emit movingChanged(moveInProgress_ = true);
	emit moveStarted();
	// in position already?
	if(inPosition()) {
		emit movingChanged(moveInProgress_ = false);
		emit moveSucceeded();
	}
	// in this case, it's harmless to re-send the value even if it's already there, since no status changes will occur; if you send open to an already-open CLS valve or shutter, nothing happens.
	// This makes sure we never omit sending it when we should.
	openPV_->setValue(1);
	return NoFailure;
}

AMControl::FailureExplanation CLSBiStateControl::close() {
	// already transitioning? Cannot send while moving.
	if(isMoving() && !allowsMovesWhileMoving()) {
		AMErrorMon::alert(this, -2, QString("Cannot close %1: it's currently in the process of opening or closing.").arg(name()));
		return AlreadyMovingFailure;
	}
	setpoint_ = 0;
	if(!moveInProgress_)
		emit movingChanged(moveInProgress_ = true);
	emit moveStarted();
	// in position already?
	if(inPosition()) {
		emit movingChanged(moveInProgress_ = false);
		emit moveSucceeded();
	}
	// in this case, it's harmless to re-send the value even if it's already there, since no status changes will occur; if you send open to an already-open CLS valve or shutter, nothing happens.
	// This makes sure we never omit sending it when we should.
	closePV_->setValue(1);
	return NoFailure;
}
