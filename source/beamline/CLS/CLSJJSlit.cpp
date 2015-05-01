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

#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/CLS/CLSJJSlitGapControl.h"
#include "beamline/CLS/CLSJJSlitCenterControl.h"

CLSJJSlit::~CLSJJSlit()
{

}

CLSJJSlit::CLSJJSlit(const QString &name, const QString &description, const QString &upperBladePVName, const QString &lowerBladePVName, const QString &inboardBladePVName, const QString &outboardBladePVName, double tolerance, double moveStartTimeoutSeconds, double limit, QObject *parent) :
	QObject(parent)
{
	name_ = name;
	description_ = description;
	limit_ = limit;

	upperBlade_ = new CLSMAXvMotor(name+"UpperBlade", upperBladePVName, description+" UpperBlade", true, tolerance, moveStartTimeoutSeconds, this);
	lowerBlade_ = new CLSMAXvMotor(name+"LowerBlade", lowerBladePVName, description+" LowerBlade", true, tolerance, moveStartTimeoutSeconds, this);
	inboardBlade_ = new CLSMAXvMotor(name+"InboardBlade", inboardBladePVName, description+" InboardBlade", true, tolerance, moveStartTimeoutSeconds, this);
	outboardBlade_ = new CLSMAXvMotor(name+"OutboardBlade", outboardBladePVName, description+" OutboardBlade", true, tolerance, moveStartTimeoutSeconds, this);

//	verticalBladesControl_ = new CLSJJSlitBladesControl(name+" Vertical", description+" Vertical", CLSJJSlitBladesControl::Vertical, verticalBladesPVBaseName, tolerance, moveStartTimeoutSeconds, this);
//	horizontalBladesControl_ = new CLSJJSlitBladesControl(name+" Horizontal", description+" Horizontal", CLSJJSlitBladesControl::Horizontal, horizontalBladesPVBaseName, tolerance, moveStartTimeoutSeconds, this);

	verticalGap_ = new CLSJJSlitGapControl(upperBlade_, lowerBlade_, this);
	verticalCenter_ = new CLSJJSlitCenterControl(upperBlade_, lowerBlade_, this);
	horizontalGap_ = new CLSJJSlitGapControl(inboardBlade_, outboardBlade_, this);
	horizontalCenter_ = new CLSJJSlitCenterControl(inboardBlade_, outboardBlade_, this);

//	connect(verticalBladesControl_, SIGNAL(connected(bool)), this, SLOT(onBladesControlConnected(bool)));
//	connect(verticalBladesControl_, SIGNAL(gapValueChanged(double)), this, SIGNAL(verticalGapValueChanged(double)));
//	connect(verticalBladesControl_, SIGNAL(centerValueChanged(double)), this, SIGNAL(verticalCenterValueChanged(double)));
//	connect(verticalBladesControl_, SIGNAL(statusStringChanged(QString)), this, SIGNAL(verticalStatusStringChanged(QString)));
//	connect(horizontalBladesControl_, SIGNAL(connected(bool)), this, SLOT(onBladesControlConnected(bool)));
//	connect(horizontalBladesControl_, SIGNAL(gapValueChanged(double)), this, SIGNAL(horizontalGapValueChanged(double)));
//	connect(horizontalBladesControl_, SIGNAL(centerValueChanged(double)), this, SIGNAL(horizontalCenterValueChanged(double)));
//	connect(horizontalBladesControl_, SIGNAL(statusStringChanged(QString)), this, SIGNAL(horizontalStatusStringChanged(QString)));

	// Listen for connected states.
	connect( verticalGap_, SIGNAL(connected(bool)), this, SLOT(onBladesControlConnected(bool)) );
	connect( verticalCenter_, SIGNAL(connected(bool)), this, SLOT(onBladesControlConnected(bool)) );
	connect( horizontalGap_, SIGNAL(connected(bool)), this, SLOT(onBladesControlConnected(bool)) );
	connect( horizontalCenter_, SIGNAL(connected(bool)), this, SLOT(onBladesControlConnected(bool)) );

	// Listen for value changes.
	connect( verticalGap_, SIGNAL(valueChanged(double)), this, SIGNAL(verticalGapValueChanged(double)) );
	connect( verticalGap_, SIGNAL(valueChanged(double)), this, SLOT(onVerticalGapChanged()) );
	connect( verticalCenter_, SIGNAL(valueChanged(double)), this, SIGNAL(verticalCenterValueChanged(double)) );
	connect( verticalCenter_, SIGNAL(valueChanged(double)), this, SLOT(onVerticalCenterChanged()) );
	connect( horizontalGap_, SIGNAL(valueChanged(double)), this, SIGNAL(horizontalGapValueChanged(double)) );
	connect( horizontalCenter_, SIGNAL(valueChanged(double)), this, SIGNAL(horizontalCenterValueChanged(double)) );

	wasConnected_ = false;
	onBladesControlConnected(false);
	onVerticalGapChanged();
	onVerticalCenterChanged();
}

bool CLSJJSlit::isConnected() const
{
//	return verticalBladesControl_->isConnected() && verticalBladesControl_->isConnected();
	return (verticalGap_->isConnected() && verticalCenter_->isConnected() && horizontalGap_->isConnected() && horizontalCenter_->isConnected());
}

void CLSJJSlit::onBladesControlConnected(bool value)
{
	Q_UNUSED(value)

	bool connectionStatus = isConnected();

	if (wasConnected_ != connectionStatus) {
		wasConnected_ = connectionStatus;
		emit connected(wasConnected_);
	}

	if (wasConnected_) {
		verticalCenter_->setGap(verticalGap_->value());
		verticalGap_->setCenterPosition(verticalCenter_->value());
	}
}

void CLSJJSlit::onVerticalGapChanged()
{
	if (isConnected()) {
		verticalCenter_->setGap(verticalGap_->value());
	}
}

void CLSJJSlit::onVerticalCenterChanged()
{
	if (isConnected()) {
		verticalGap_->setCenterPosition(verticalCenter_->value());
	}
}

//void CLSJJSlit::moveVerticalGap(double setpoint)
//{
//	verticalBladesControl_->moveGap(setpoint);
//}

//void CLSJJSlit::moveVerticalCenter(double setpoint)
//{
//	verticalBladesControl_->moveCenter(setpoint);
//}

//void CLSJJSlit::moveHorizontalGap(double setpoint)
//{
//	horizontalBladesControl_->moveGap(setpoint);
//}

//void CLSJJSlit::moveHorizontalCenter(double setpoint)
//{
//	horizontalBladesControl_->moveCenter(setpoint);
//}
