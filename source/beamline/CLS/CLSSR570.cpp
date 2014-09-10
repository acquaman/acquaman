#include "CLSSR570.h"

#include <QDebug>
#include "beamline/AMPVControl.h"

CLSSR570::CLSSR570(const QString &name, const QString &baseName, QObject *parent) :
	AMCurrentAmplifier(name, parent)
{
	sensitivityNums_ << 1 << 2 << 5 << 10 << 20 << 50 << 100 << 200 << 500;
	sensitivityUnits_ << "pA/V" << "nA/V" << "uA/V" << "mA/V";

	supportsSensitivityMode_ = true;

	atMinimumSensitivity_ = false;
	atMaximumSensitivity_ = false;

	// sensitivityNumControl_ and senstivityUnitControl_ must be initialized before sensitivityControl_
	sensitivityNumControl_ = new AMSinglePVControl(QString("%1Sensitivity").arg(name), QString("%1:sens_num").arg(baseName), this, 0.5);
	connect(sensitivityNumControl_, SIGNAL(connected(bool)), this, SLOT(onSensitivityNumControlConnectedChanged(bool)));
	connect(sensitivityNumControl_, SIGNAL(valueChanged(double)), this, SLOT(onSensitivityControlNumValueChanged(double)));

	sensitivityUnitControl_ = new AMSinglePVControl(QString("%1Sensitivity").arg(name), QString("%1:sens_unit").arg(baseName), this, 0.5);
	connect(sensitivityUnitControl_, SIGNAL(connected(bool)), this, SLOT(onSensitivityControlUnitConnectedChanged(QString)));
	connect(sensitivityUnitControl_, SIGNAL(valueChanged(QString)), this, SLOT(onSensitivityControlUnitValueChanged(QString)));

	sensitivityControl_ = new AMSinglePVControl(QString("%1Sensitivity").arg(name), QString("%1:sens_put").arg(baseName), this, 0.5);
	connect(sensitivityControl_, SIGNAL(connected(bool)), this, SLOT(onSensitivityControlConnectedChanged(bool)));
	connect(sensitivityControl_, SIGNAL(valueChanged(double)), this, SLOT(onSensitivityControlValueChanged(double)));

	setAmplifierMode(AMCurrentAmplifier::Sensitivity);
}

CLSSR570::~CLSSR570(){}

int CLSSR570::sensitivityLevel() const
{
	return (int)sensitivityControl_->value();
}

double CLSSR570::value() const
{
	return currentValueHelper();
}

QList<double> CLSSR570::values() const
{
	return sensitivityNums_;
}

QString CLSSR570::units() const
{
	return currentUnitHelper();
}

QStringList CLSSR570::unitsList() const
{
	return sensitivityUnits_;
}

double CLSSR570::minimumValueForUnits(const QString &units) const
{
	Q_UNUSED(units)

	return 1;
}

double CLSSR570::maximumValueForUnits(const QString &units) const
{
	int value;

	if (units == "mA/V")
		value = 1;
	else
		value = 500;

	return value;
}

void CLSSR570::onSensitivityControlConnectedChanged(bool connected)
{
	if(connected != connected_){

		connected_ = connected;
		emit isConnected(connected_);
	}
}

void CLSSR570::onSensitivityControlValueChanged(double controlValue)
{
	atMinimumSensitivity_ = false;
	atMaximumSensitivity_ = false;
	if (controlValue == 0)
		atMaximumSensitivity_ = true;
	else if (controlValue == 27)
		atMinimumSensitivity_ = true;

	// update the value of sensitivity num pv and sensitivity unit pv
	sensitivityNumControl_->disableProcessRecord();
	sensitivityNumControl_->move(sensitivityNums_.indexOf(value()));
	sensitivityNumControl_->enableProcessRecord();

	sensitivityUnitControl_->disableProcessRecord();
	sensitivityUnitControl_->move(sensitivityUnits_.indexOf(units()));
	sensitivityUnitControl_->enableProcessRecord();

	emit valueChanged();
}

void CLSSR570::onSensitivityNumControlConnectedChanged(bool connected)
{
	if(connected != connected_){

		connected_ = connected;
		emit isConnected(connected_);
	}
}

void CLSSR570::onSensitivityNumControlValueChanged(double value)
{
	emit valueChanged();

}
void CLSSR570::onSensitivityUnitControlConnectedChanged(bool connected)
{
	if(connected != connected_){

		connected_ = connected;
		emit isConnected(connected_);
	}
}

void CLSSR570::onSensitivityUnitControlValueChanged(QString value)
{
	emit valueChanged();
}

bool CLSSR570::atMinimumSensitivity() const
{
	return atMinimumSensitivity_;
}

bool CLSSR570::atMaximumSensitivity() const
{
	return atMaximumSensitivity_;
}

bool CLSSR570::increaseSensitivity()
{
	if(atMaximumSensitivity_)
		return false;

	double currentSensitivityControlValue = sensitivityControl_->value();
	AMControl::FailureExplanation failureExplanation = sensitivityControl_->move(currentSensitivityControlValue-1);

	return (failureExplanation == AMControl::NoFailure);
}

bool CLSSR570::decreaseSensitivity()
{
	if(atMinimumSensitivity_)
		return false;

	double currentSensitivityControlValue = sensitivityControl_->value();
	AMControl::FailureExplanation failureExplanation = sensitivityControl_->move(currentSensitivityControlValue+1);

	return (failureExplanation == AMControl::NoFailure);
}

void CLSSR570::setValueImplementation(const QString &valueArg)
{
	if(valueArg == "1 pA/V")
		sensitivityControl_->move(0);

	else if(valueArg == "2 pA/V")
		sensitivityControl_->move(1);

	else if(valueArg == "5 pA/V")
		sensitivityControl_->move(2);

	else if(valueArg == "10 pA/V")
		sensitivityControl_->move(3);

	else if(valueArg == "20 pA/V")
		sensitivityControl_->move(4);

	else if(valueArg == "50 pA/V")
		sensitivityControl_->move(5);

	else if(valueArg == "100 pA/V")
		sensitivityControl_->move(6);

	else if(valueArg == "200 pA/V")
		sensitivityControl_->move(7);

	else if(valueArg == "500 pA/V")
		sensitivityControl_->move(8);

	else if(valueArg == "1 nA/V")
		sensitivityControl_->move(9);

	else if(valueArg == "2 nA/V")
		sensitivityControl_->move(10);

	else if(valueArg == "5 nA/V")
		sensitivityControl_->move(11);

	else if(valueArg == "10 nA/V")
		sensitivityControl_->move(12);

	else if(valueArg == "20 nA/V")
		sensitivityControl_->move(13);

	else if(valueArg == "50 nA/V")
		sensitivityControl_->move(14);

	else if(valueArg == "100 nA/V")
		sensitivityControl_->move(15);

	else if(valueArg == "200 nA/V")
		sensitivityControl_->move(16);

	else if(valueArg == "500 nA/V")
		sensitivityControl_->move(17);

	else if(valueArg == "1 uA/V")
		sensitivityControl_->move(18);

	else if(valueArg == "2 uA/V")
		sensitivityControl_->move(19);

	else if(valueArg == "5 uA/V")
		sensitivityControl_->move(20);

	else if(valueArg == "10 uA/V")
		sensitivityControl_->move(21);

	else if(valueArg == "20 uA/V")
		sensitivityControl_->move(22);

	else if(valueArg == "50 uA/V")
		sensitivityControl_->move(23);

	else if(valueArg == "100 uA/V")
		sensitivityControl_->move(24);

	else if(valueArg == "200 uA/V")
		sensitivityControl_->move(25);

	else if(valueArg == "500 uA/V")
		sensitivityControl_->move(26);

	else if(valueArg == "1 mA/V")
		sensitivityControl_->move(27);

	return;
}

double CLSSR570::currentValueHelper() const
{
	int currentSensitivityControlValue = (int)sensitivityControl_->value();

	switch(currentSensitivityControlValue){

	case 0:
		return 1;

	case 1:
		return 2;

	case 2:
		return 5;

	case 3:
		return 10;

	case 4:
		return 20;

	case 5:
		return 50;

	case 6:
		return 100;

	case 7:
		return 200;

	case 8:
		return 500;

	case 9:
		return 1;

	case 10:
		return 2;

	case 11:
		return 5;

	case 12:
		return 10;

	case 13:
		return 20;

	case 14:
		return 50;

	case 15:
		return 100;

	case 16:
		return 200;

	case 17:
		return 500;

	case 18:
		return 1;

	case 19:
		return 2;

	case 20:
		return 5;

	case 21:
		return 10;

	case 22:
		return 20;

	case 23:
		return 50;

	case 24:
		return 100;

	case 25:
		return 200;

	case 26:
		return 500;

	case 27:
		return 1;

	default:
		return -1;
	}
}

QString CLSSR570::currentUnitHelper() const
{
	int currentSensitivityControlValue = (int)sensitivityControl_->value();

	if(currentSensitivityControlValue <= 8)
		return "pA/V";

	else if(currentSensitivityControlValue <= 17)
		return "nA/V";

	else if(currentSensitivityControlValue <= 26)
		return "uA/V";

	else
		return "mA/V";
}
