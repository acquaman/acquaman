#include "CLSJJSlit.h"

CLSJJSlit::~CLSJJSlit()
{

}

CLSJJSlit::CLSJJSlit(const QString &name, const QString &description, const QString &pvBaseName, double tolerance, double moveStartTimeoutSeconds, QObject *parent):
	QObject(parent)
{
	name_ = name;
	description_ = description;

	gapPVControl_ = new AMPVwStatusControl(name+" Gap PV", pvBaseName+":gap:fbk", pvBaseName+":gap", pvBaseName+":gap", pvBaseName+":status", this, tolerance, moveStartTimeoutSeconds, new CLSJJSlitControlStatusChecker());
	centerPVControl_ = new AMPVwStatusControl(name+" Center PV", pvBaseName+":center:fbk", pvBaseName+":center", pvBaseName+":center", pvBaseName+":status", this, tolerance, moveStartTimeoutSeconds, new CLSJJSlitControlStatusChecker());
	statusPVControl_ = new AMReadOnlyPVControl(name+" Status PV", pvBaseName+":status", this);

	connect(gapPVControl_, SIGNAL(connected(bool)), this, SLOT(onPVControlConnected(bool)));
	connect(gapPVControl_, SIGNAL(valueChanged(double)), this, SIGNAL(gapValueChanged(double)));
	connect(centerPVControl_, SIGNAL(connected(bool)), this, SLOT(onPVControlConnected(bool)));
	connect(centerPVControl_, SIGNAL(valueChanged(double)), this, SIGNAL(centerValueChanged(double)));
	connect(statusPVControl_, SIGNAL(connected(bool)), this, SLOT(onPVControlConnected(bool)));
	connect(statusPVControl_, SIGNAL(valueChanged(double)), this, SIGNAL(statusValueChanged(double)));

	wasConnected_ = false;
	onPVControlConnected(false);
}

bool CLSJJSlit::isConnected()
{
	return gapPVControl_->isConnected() && centerPVControl_->isConnected() && statusPVControl_->isConnected();
}

double CLSJJSlit::gap()
{
	return gapPVControl_->value();
}

double CLSJJSlit::center()
{
	return centerPVControl_->value();
}

double CLSJJSlit::status()
{
	return statusPVControl_->value();
}

void CLSJJSlit::onPVControlConnected(bool value)
{
	Q_UNUSED(value)

	bool connectionStatus = isConnected();

	if (wasConnected_ != connectionStatus) {
		wasConnected_ = connectionStatus;
		emit connected(wasConnected_);
	}
}

void CLSJJSlit::moveGap(double setpoint)
{
	gapPVControl_->move(setpoint);
}

void CLSJJSlit::moveCenter(double setpoint)
{
	centerPVControl_->move(setpoint);
}
