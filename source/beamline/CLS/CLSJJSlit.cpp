#include "CLSJJSlit.h"

// ======================== implementation for CLSJJSlitBladesControl ======================================
CLSJJSlitBladesControl::~CLSJJSlitBladesControl()
{

}

CLSJJSlitBladesControl::CLSJJSlitBladesControl(const QString &name, const QString &description,  const Direction bladeDirection, const QString &pvBaseName, double tolerance, double moveStartTimeoutSeconds, QObject *parent):
	QObject(parent)
{
	name_ = name;
	description_ = description;
	direction_ = bladeDirection;

//	gapPVControl_ = new AMPVwStatusControl(name+" Gap", pvBaseName+":gap:fbk", pvBaseName+":gap", pvBaseName+":gap", pvBaseName+":status", this, tolerance, moveStartTimeoutSeconds, new CLSJJSlitControlStatusChecker());
//	centerPVControl_ = new AMPVwStatusControl(name+" Center", pvBaseName+":center:fbk", pvBaseName+":center", pvBaseName+":center", pvBaseName+":status", this, tolerance, moveStartTimeoutSeconds, new CLSJJSlitControlStatusChecker());
//	statusPVControl_ = new AMReadOnlyPVControl(name+" Status", pvBaseName+":status", this);
	gapPVControl_ = new AMPVControl(name+" Gap", pvBaseName+":gap:fbk", pvBaseName+":gap", QString(), this, tolerance, moveStartTimeoutSeconds);
	centerPVControl_ = new AMPVControl(name+" Center", pvBaseName+":center:fbk", pvBaseName+":center", QString(), this, tolerance, moveStartTimeoutSeconds);
	statusPVControl_ = new AMReadOnlyPVControl(name+" Status", pvBaseName+":status", this);

	connect(gapPVControl_, SIGNAL(connected(bool)), this, SLOT(onPVControlConnected()));
	connect(gapPVControl_, SIGNAL(valueChanged(double)), this, SIGNAL(gapValueChanged(double)));
	connect(centerPVControl_, SIGNAL(connected(bool)), this, SLOT(onPVControlConnected()));
	connect(centerPVControl_, SIGNAL(valueChanged(double)), this, SIGNAL(centerValueChanged(double)));
	connect(statusPVControl_, SIGNAL(connected(bool)), this, SLOT(onPVControlConnected()));
	connect(statusPVControl_, SIGNAL(valueChanged(double)), this, SLOT(onStatusValueChanged()));

	wasConnected_ = false;
	onPVControlConnected();
}

bool CLSJJSlitBladesControl::isConnected() const
{
	return gapPVControl_->isConnected() && centerPVControl_->isConnected() && statusPVControl_->isConnected();
}

void CLSJJSlitBladesControl::onPVControlConnected()
{
	bool connectionStatus = isConnected();

	if (wasConnected_ != connectionStatus) {
		wasConnected_ = connectionStatus;
		emit connected(wasConnected_);
	}
}

void CLSJJSlitBladesControl::moveGap(double setpoint)
{
	gapPVControl_->move(setpoint);
}

void CLSJJSlitBladesControl::moveCenter(double setpoint)
{
	centerPVControl_->move(setpoint);
}

void CLSJJSlitBladesControl::onStatusValueChanged()
{
	emit statusStringChanged(statusString());
}

// ======================== implementation for CLSJJSlit ======================================
CLSJJSlit::~CLSJJSlit()
{

}

CLSJJSlit::CLSJJSlit(const QString &name, const QString &description, const QString &verticalBladesPVBaseName, const QString &horizontalBladesPVBaseName, double tolerance, double moveStartTimeoutSeconds, double limit, QObject *parent):
	QObject(parent)
{
	name_ = name;
	description_ = description;

	verticalBladesControl_ = new CLSJJSlitBladesControl(name+" Vertical Blades", description+" Vertical Blades", CLSJJSlitBladesControl::Vertical, verticalBladesPVBaseName, tolerance, moveStartTimeoutSeconds, this);
	horizontalBladesControl_ = new CLSJJSlitBladesControl(name+" Horizontal Blades", description+" Horizontal Blades", CLSJJSlitBladesControl::Horizontal, horizontalBladesPVBaseName, tolerance, moveStartTimeoutSeconds, this);

	connect(verticalBladesControl_, SIGNAL(connected(bool)), this, SLOT(onBladesControlConnected(bool)));
	connect(verticalBladesControl_, SIGNAL(gapValueChanged(double)), this, SIGNAL(verticalGapValueChanged(double)));
	connect(verticalBladesControl_, SIGNAL(centerValueChanged(double)), this, SIGNAL(verticalCenterValueChanged(double)));
	connect(verticalBladesControl_, SIGNAL(statusStringChanged(QString)), this, SIGNAL(verticalStatusStringChanged(QString)));
	connect(horizontalBladesControl_, SIGNAL(connected(bool)), this, SLOT(onBladesControlConnected(bool)));
	connect(horizontalBladesControl_, SIGNAL(gapValueChanged(double)), this, SIGNAL(horizontalGapValueChanged(double)));
	connect(horizontalBladesControl_, SIGNAL(centerValueChanged(double)), this, SIGNAL(horizontalCenterValueChanged(double)));
	connect(horizontalBladesControl_, SIGNAL(statusStringChanged(QString)), this, SIGNAL(horizontalStatusStringChanged(QString)));

	wasConnected_ = false;
	onBladesControlConnected(false);
}

bool CLSJJSlit::isConnected() const
{
	return verticalBladesControl_->isConnected() && verticalBladesControl_->isConnected();
}

void CLSJJSlit::onBladesControlConnected(bool value)
{
	Q_UNUSED(value)

	bool connectionStatus = isConnected();

	if (wasConnected_ != connectionStatus) {
		wasConnected_ = connectionStatus;
		emit connected(wasConnected_);
	}
}

void CLSJJSlit::moveVerticalGap(double setpoint)
{
	verticalBladesControl_->moveGap(setpoint);
}

void CLSJJSlit::moveVerticalCenter(double setpoint)
{
	verticalBladesControl_->moveCenter(setpoint);
}

void CLSJJSlit::moveHorizontalGap(double setpoint)
{
	horizontalBladesControl_->moveGap(setpoint);
}

void CLSJJSlit::moveHorizontalCenter(double setpoint)
{
	horizontalBladesControl_->moveCenter(setpoint);
}
