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


#include "CLSSIS3820ScalerSADetector.h"

#include "beamline/AMPVControl.h"
#include <QStringBuilder>


CLSSIS3820ScalerSADetector::CLSSIS3820ScalerSADetector(const QString &name, const QString &description, const QString &pvBaseName, int channelNumber, bool sendTrigger, QObject* parent, double initializeTimeoutSeconds)
	: AMSADetector(name, description, parent)
{
	measurementInfo_.units = "counts";

	initializationFinished_ = false;
	initializationSucceeded_ = false;

	wasConnected_ = false;

	isAcquiring_ = false;

	lastAcquisitionFinished_ = false;
	lastAcquisitionSucceeded_ = false;

	sendTrigger_ = sendTrigger;
	channelNumber_ = channelNumber;
	data_ = 0;

	initializeTimeoutSeconds_ = initializeTimeoutSeconds;

	QString channelString = QString("%1").arg(channelNumber_, 2, 10, QChar('0'));

	delayPV_ = new AMSinglePVControl(name % "_delay", pvBaseName % ":delay", this);
	startPV_ = new AMSinglePVControl(name % "_startScan", pvBaseName % ":startScan", this);
	continuousPV_ = new AMSinglePVControl(name % "_continuous", pvBaseName % ":continuous", this);
	enablePV_ = new AMSinglePVControl(name % "_enable", pvBaseName % channelString % ":enable", this);
	feedbackPV_ = new AMSinglePVControl(name % "_feedback", pvBaseName % channelString % ":fbk", this);

	// monitor connection status; emit connected().
	connect(delayPV_, SIGNAL(connected(bool)), this, SLOT(onPVConnected()));
	connect(startPV_, SIGNAL(connected(bool)), this, SLOT(onPVConnected()));
	connect(continuousPV_, SIGNAL(connected(bool)), this, SLOT(onPVConnected()));
	connect(enablePV_, SIGNAL(connected(bool)), this, SLOT(onPVConnected()));
	connect(feedbackPV_, SIGNAL(connected(bool)), this, SLOT(onPVConnected()));

	connect(feedbackPV_, SIGNAL(valueChanged(double)), this, SLOT(onFeedbackValueChanged(double)));
}

CLSSIS3820ScalerSADetector::~CLSSIS3820ScalerSADetector() {
	// PVs are deleted as QObject children.
}

bool CLSSIS3820ScalerSADetector::init()
{
	initializationFinished_ = false;
	initializationSucceeded_ = false;

	if(!isConnected()) {
		initializationFinished_ = true;
		return false;
	}

	connect(enablePV_, SIGNAL(valueChanged(double)), this, SLOT(reviewInitialized()));
	connect(continuousPV_, SIGNAL(valueChanged(double)), this, SLOT(reviewInitialized()));
	continuousPV_->move(0);
	enablePV_->move(1);

	/// \todo Move initialization timeout to scan controller? Make sure this cannot get called multiple times over top of itself? What about cancelling?
	QTimer::singleShot(initializeTimeoutSeconds_*1000, this, SLOT(onInitializationTimeout()));

	reviewInitialized();

	return true;
}

void CLSSIS3820ScalerSADetector::reviewInitialized()
{
	if(continuousPV_->value() == 0.0 && enablePV_->value() == 1.0) {
		disconnect(enablePV_, SIGNAL(valueChanged(double)), this, SLOT(reviewInitialized()));
		disconnect(continuousPV_, SIGNAL(valueChanged(double)), this, SLOT(reviewInitialized()));

		initializationFinished_ = true;
		initializationSucceeded_ = true;
		emit initialized(true);
	}
}

void CLSSIS3820ScalerSADetector::onInitializationTimeout()
{
	if(!initializationFinished_ && !initializationSucceeded_) {
		disconnect(enablePV_, SIGNAL(valueChanged(double)), this, SLOT(reviewInitialized()));
		disconnect(continuousPV_, SIGNAL(valueChanged(double)), this, SLOT(reviewInitialized()));

		initializationFinished_ = true;
		initializationSucceeded_ = false;
		emit initialized(false);
	}
}



double CLSSIS3820ScalerSADetector::acquisitionTime() const
{
	return delayPV_->value()/1000.0;
}

bool CLSSIS3820ScalerSADetector::setAcquisitionTime(double seconds)
{
	return delayPV_->move(seconds*1000.0) == AMControl::NoFailure;
}

bool CLSSIS3820ScalerSADetector::isConnected() const
{
	return delayPV_->isConnected() &&
			startPV_->isConnected() &&
			continuousPV_->isConnected() &&
			enablePV_->isConnected() &&
			feedbackPV_->isConnected();
}

bool CLSSIS3820ScalerSADetector::acquireNow()
{
	if(!initializationSucceeded_ || isAcquiring_)
		return false;

	isAcquiring_ = true;
	lastAcquisitionFinished_ = false;
	lastAcquisitionSucceeded_ = false;

	if(sendTrigger_)
		return startPV_->move(1) == AMControl::NoFailure;
	else
		return true;
}

AMNumber CLSSIS3820ScalerSADetector::value(const AMnDIndex &index) const
{
	if(index.rank() != 0)
		return AMNumber::DimensionError;

	return data_;
}

const double * CLSSIS3820ScalerSADetector::data() const
{
	return &data_;
}

void CLSSIS3820ScalerSADetector::onFeedbackValueChanged(double newValue)
{
	if(!isAcquiring_)
		return;

	isAcquiring_ = false;

	lastAcquisitionFinished_ = true;
	lastAcquisitionSucceeded_ = true;
	data_ = newValue;

	emit acquisitionFinished(true);
}

void CLSSIS3820ScalerSADetector::cancelAcquisition()
{
	if(!isAcquiring_)
		return;

	if(sendTrigger_)
		startPV_->move(0);

	isAcquiring_ = false;
	lastAcquisitionFinished_ = true;
	lastAcquisitionSucceeded_ = false;
}

void CLSSIS3820ScalerSADetector::onPVConnected()
{
	bool nowConnected = isConnected();
	if(wasConnected_ != nowConnected)
		emit connected(wasConnected_ = nowConnected);
}

