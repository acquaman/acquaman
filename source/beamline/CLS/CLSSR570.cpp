#include "CLSSR570.h"

#include "beamline/AMPVControl.h"

CLSSR570::CLSSR570(const QString &name, const QString &baseName, QObject *parent) :
	AMCurrentAmplifier(name, parent)
{
	sensitivityNums_ << 1 << 2 << 5 << 10 << 20 << 50 << 100 << 200 << 500;
	sensitivityUnits_ << "pA/V" << "nA/V" << "uA/V" << "mA/V";
	sensitivityValueArgs_ << "1 pA/V" << "2 pA/V" << "5 pA/V" << "10 pA/V" << "20 pA/V" << "50 pA/V" << "100 pA/V" << "200 pA/V" << "500 pA/V" \
						  << "1 nA/V" << "2 nA/V" << "5 nA/V" << "10 nA/V" << "20 nA/V" << "50 nA/V" << "100 nA/V" << "200 nA/V" << "500 nA/V" \
						  << "1 uA/V" << "2 uA/V" << "5 uA/V" << "10 uA/V" << "20 uA/V" << "50 uA/V" << "100 uA/V" << "200 uA/V" << "500 uA/V" \
						  << "1 mA/V";

	supportsSensitivityMode_ = true;

	atMinimumSensitivity_ = false;
	atMaximumSensitivity_ = false;

	// sensitivityNumControl_ and senstivityUnitControl_ must be initialized before sensitivityControl_
	sensitivityNumControl_ = new AMSinglePVControl(QString("%1Sensitivity").arg(name), QString("%1:sens_num").arg(baseName), this, 0.5);
	sensitivityUnitControl_ = new AMSinglePVControl(QString("%1Sensitivity").arg(name), QString("%1:sens_unit").arg(baseName), this, 0.5);

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

void CLSSR570::onSensitivityControlValueChanged(double value)
{	
	if(fabs(value - 0) < 0.5 && !atMaximumSensitivity_){

		atMaximumSensitivity_ = true;
		emit maximumValue(true);
	}

	else if(fabs(value - 27) < 0.5 && !atMinimumSensitivity_) {

		atMinimumSensitivity_ = true;
		emit minimumValue(true);
	}

	else if(atMaximumSensitivity_ && !(fabs(value - 0) < 0.5)){

		atMaximumSensitivity_ = false;
		emit maximumValue(false);
	}

	else if(atMinimumSensitivity_ && !(fabs(value - 27) < 0.5)){

		atMinimumSensitivity_ = false;
		emit minimumValue(false);
	}

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
	int index = sensitivityValueArgs_.indexOf(valueArg);
	sensitivityControl_->move(index);

	return;
}

double CLSSR570::currentValueHelper() const
{
	int currentSensitivityControlValue = (int)sensitivityControl_->value();

	if ((currentSensitivityControlValue >= 0) && (currentSensitivityControlValue <= 27)) {
		return sensitivityNums_.at(currentSensitivityControlValue % 9);
	} else
		return -1;
}

QString CLSSR570::currentUnitHelper() const
{
	int currentSensitivityControlValue = (int)sensitivityControl_->value();

	if ((currentSensitivityControlValue >= 0) && (currentSensitivityControlValue <= 27)) {
		return sensitivityUnits_.at(currentSensitivityControlValue / 9);
	} else
		return "mA/V";
}
