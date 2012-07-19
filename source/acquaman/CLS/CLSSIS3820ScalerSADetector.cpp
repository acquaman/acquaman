#include "CLSSIS3820ScalerSADetector.h"

#include "beamline/AMPVControl.h"
#include <QStringBuilder>

CLSSIS3820ScalerSADetector::CLSSIS3820ScalerSADetector(const QString &name, const QString &description, const QString &pvBaseName, int channelNumber, bool sendTrigger, QObject* parent, double initializeTimeoutSeconds)
	: AMSADetector(name, description, parent)
{
	measurementInfo_.units = "counts";

	isAcquiring_ = 0;
	isAcquisitionFinished_ = 0;
	isInitialized_ = 0;

	sendTrigger_ = sendTrigger;
	channelNumber_ = channelNumber;
	data_ = 0;

	initializeTimeoutSeconds_ = initializeTimeoutSeconds;

	QString channelString = QString("%1").arg(channelNumber_, 2, 10, QChar('0'));

	delayPV_ = new AMSinglePVControl(name % "_delay", pvBaseName % ":enable", this);
	startPV_ = new AMSinglePVControl(name % "_startScan", pvBaseName % ":startScan", this);
	continuousPV_ = new AMSinglePVControl(name % "_continuous", pvBaseName % ":continuous", this);
	enablePV_ = new AMSinglePVControl(name % "_enable", pvBaseName % channelString % ":enable", this);
	feedbackPV_ = new AMSinglePVControl(name % "_feedback", pvBaseName % channelString % ":fbk", this);

	connect(feedbackPV_, SIGNAL(valueChanged(double)), this, SLOT(onFeedbackValueChanged(double)));
}

CLSSIS3820ScalerSADetector::~CLSSIS3820ScalerSADetector() {
	// PVs are deleted as QObject children.
}

bool CLSSIS3820ScalerSADetector::init()
{
	isInitialized_ = false;

	connect(delayPV_, SIGNAL(connected(bool)), this, SLOT(reviewConnected()));
	connect(startPV_, SIGNAL(connected(bool)), this, SLOT(reviewConnected()));
	connect(continuousPV_, SIGNAL(connected(bool)), this, SLOT(reviewConnected()));
	connect(enablePV_, SIGNAL(connected(bool)), this, SLOT(reviewConnected()));
	connect(feedbackPV_, SIGNAL(connected(bool)), this, SLOT(reviewConnected()));

	QTimer::singleShot(initializeTimeoutSeconds_*1000, this, SLOT(onInitializationTimeout()));

	reviewConnected();

	return true;
}

void CLSSIS3820ScalerSADetector::reviewConnected()
{
	if(isConnected()) {
		disconnect(delayPV_, SIGNAL(connected(bool)), this, SLOT(reviewConnected()));
		disconnect(startPV_, SIGNAL(connected(bool)), this, SLOT(reviewConnected()));
		disconnect(continuousPV_, SIGNAL(connected(bool)), this, SLOT(reviewConnected()));
		disconnect(enablePV_, SIGNAL(connected(bool)), this, SLOT(reviewConnected()));
		disconnect(feedbackPV_, SIGNAL(connected(bool)), this, SLOT(reviewConnected()));

		connect(enablePV_, SIGNAL(valueChanged(double)), this, SLOT(reviewInitialized()));
		connect(continuousPV_, SIGNAL(valueChanged(double)), this, SLOT(reviewInitialized()));
		continuousPV_->move(0);
		enablePV_->move(1);

		reviewInitialized();
	}
}


void CLSSIS3820ScalerSADetector::reviewInitialized()
{
	if(continuousPV_->value() == 0.0 && enablePV_->value() == 1.0) {
		disconnect(enablePV_, SIGNAL(valueChanged(double)), this, SLOT(reviewInitialized()));
		disconnect(continuousPV_, SIGNAL(valueChanged(double)), this, SLOT(reviewInitialized()));

		isInitialized_ = true;
		emit initialized(true);
	}
}

void CLSSIS3820ScalerSADetector::onInitializationTimeout()
{
	if(!isInitialized_) {
		disconnect(delayPV_, 0, this, 0);
		disconnect(startPV_, 0, this, 0);
		disconnect(continuousPV_, 0, this, 0);
		disconnect(enablePV_, 0, this, 0);
		disconnect(feedbackPV_, 0, this, 0);

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
	if(!isInitialized_ || isAcquiring_)
		return false;

	isAcquiring_ = true;
	isAcquisitionFinished_ = false;

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
	isAcquisitionFinished_ = true;
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
}

