#include "CLSSR570.h"

#include "beamline/AMPVControl.h"

CLSSR570::CLSSR570(const QString &name, const QString &baseName, QObject *parent) :
	AMCurrentAmplifier(name, parent)
{
	supportsSensitivityMode_ = true;

	atMinimumSensitivity_ = false;
	atMaximumSensitivity_ = false;

	sensitivityControl_ = new AMSinglePVControl(QString("%1Sensitivity").arg(name), QString("%1:sens_put").arg(baseName), this, 0.5);

	connect(sensitivityControl_, SIGNAL(connected(bool)), this, SLOT(onSensitivityControlConnectedChanged(bool)));
	connect(sensitivityControl_, SIGNAL(valueChanged(double)), this, SLOT(onSensitivityControlValueChanged(double)));

	setAmplifierMode(AMCurrentAmplifier::Sensitivity);
}

CLSSR570::~CLSSR570(){}

double CLSSR570::value() const{
	return currentValueHelper();
}

QList<double> CLSSR570::values() const
{
	return QList<double>() << 1 << 2 << 5 << 10 << 20 << 50 << 100 << 200 << 500;
}

QString CLSSR570::units() const{
	return currentUnitHelper();
}

QStringList CLSSR570::unitsList() const
{
	return QStringList() << "pA/V" << "nA/V" << "uA/V" << "mA/V";
}

double CLSSR570::minimumValueForUnits(const QString &units) const{
	Q_UNUSED(units)

	return 1;
}

double CLSSR570::maximumValueForUnits(const QString &units) const{
	int value;

	if (units == "mA/V")
	    value = 1;
	else
	    value = 500;

	return value;
}

void CLSSR570::onSensitivityControlConnectedChanged(bool connected){
	if(connected != connected_){
		connected_ = connected;
		emit isConnected(connected_);
	}
}

void CLSSR570::onSensitivityControlValueChanged(double value){
	emit valueChanged();
	if(fabs(value - 0) < 0.5 && !atMaximumSensitivity_){
		atMaximumSensitivity_ = true;
		emit maximumValue(true);
	}
	else if(fabs(value - 27) < 0.5 && !atMinimumSensitivity_){
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
}

bool CLSSR570::atMinimumSensitivity() const{
	return atMinimumSensitivity_;
}

bool CLSSR570::atMaximumSensitivity() const{
	return atMaximumSensitivity_;
}

bool CLSSR570::increaseSensitivity(){
	if(atMaximumSensitivity_)
		return false;

	double currentSensitivityControlValue = sensitivityControl_->value();
	AMControl::FailureExplanation failureExplanation = sensitivityControl_->move(currentSensitivityControlValue-1);
	if(failureExplanation == AMControl::NoFailure)
		return true;
	return false;
}

bool CLSSR570::decreaseSensitivity(){
	if(atMinimumSensitivity_)
		return false;

	double currentSensitivityControlValue = sensitivityControl_->value();
	AMControl::FailureExplanation failureExplanation = sensitivityControl_->move(currentSensitivityControlValue+1);
	if(failureExplanation == AMControl::NoFailure)
		return true;
	return false;
}

void CLSSR570::setValueImplementation(const QString &valueArg)
{
	if(valueArg == "1 pA/V"){
		sensitivityControl_->move(0);
	}
	else if(valueArg == "2 pA/V"){
		sensitivityControl_->move(1);
	}
	else if(valueArg == "5 pA/V"){
		sensitivityControl_->move(2);
	}
	else if(valueArg == "10 pA/V"){
		sensitivityControl_->move(3);
	}
	else if(valueArg == "20 pA/V"){
		sensitivityControl_->move(4);
	}
	else if(valueArg == "50 pA/V"){
		sensitivityControl_->move(5);
	}
	else if(valueArg == "100 pA/V"){
		sensitivityControl_->move(6);
	}
	else if(valueArg == "200 pA/V"){
		sensitivityControl_->move(7);
	}
	else if(valueArg == "500 pA/V"){
		sensitivityControl_->move(8);
	}
	else if(valueArg == "1 nA/V"){
		sensitivityControl_->move(9);
	}
	else if(valueArg == "2 nA/V"){
		sensitivityControl_->move(10);
	}
	else if(valueArg == "5 nA/V"){
		sensitivityControl_->move(11);
	}
	else if(valueArg == "10 nA/V"){
		sensitivityControl_->move(12);
	}
	else if(valueArg == "20 nA/V"){
		sensitivityControl_->move(13);
	}
	else if(valueArg == "50 nA/V"){
		sensitivityControl_->move(14);
	}
	else if(valueArg == "100 nA/V"){
		sensitivityControl_->move(15);
	}
	else if(valueArg == "200 nA/V"){
		sensitivityControl_->move(16);
	}
	else if(valueArg == "500 nA/V"){
		sensitivityControl_->move(17);
	}
	else if(valueArg == "1 uA/V"){
		sensitivityControl_->move(18);
	}
	else if(valueArg == "2 uA/V"){
		sensitivityControl_->move(19);
	}
	else if(valueArg == "5 uA/V"){
		sensitivityControl_->move(20);
	}
	else if(valueArg == "10 uA/V"){
		sensitivityControl_->move(21);
	}
	else if(valueArg == "20 uA/V"){
		sensitivityControl_->move(22);
	}
	else if(valueArg == "50 uA/V"){
		sensitivityControl_->move(23);
	}
	else if(valueArg == "100 uA/V"){
		sensitivityControl_->move(24);
	}
	else if(valueArg == "200 uA/V"){
		sensitivityControl_->move(25);
	}
	else if(valueArg == "500 uA/V"){
		sensitivityControl_->move(26);
	}
	else if(valueArg == "1 mA/V"){
		sensitivityControl_->move(27);
	}

	return;
}

double CLSSR570::currentValueHelper() const{
	int currentSensitivityControlValue = (int)sensitivityControl_->value();
	switch(currentSensitivityControlValue){
	case 0:
		return 1;
		break;
	case 1:
		return 2;
		break;
	case 2:
		return 5;
		break;
	case 3:
		return 10;
		break;
	case 4:
		return 20;
		break;
	case 5:
		return 50;
		break;
	case 6:
		return 100;
		break;
	case 7:
		return 200;
		break;
	case 8:
		return 500;
		break;
	case 9:
		return 1;
		break;
	case 10:
		return 2;
		break;
	case 11:
		return 5;
		break;
	case 12:
		return 10;
		break;
	case 13:
		return 20;
		break;
	case 14:
		return 50;
		break;
	case 15:
		return 100;
		break;
	case 16:
		return 200;
		break;
	case 17:
		return 500;
		break;
	case 18:
		return 1;
		break;
	case 19:
		return 2;
		break;
	case 20:
		return 5;
		break;
	case 21:
		return 10;
		break;
	case 22:
		return 20;
		break;
	case 23:
		return 50;
		break;
	case 24:
		return 100;
		break;
	case 25:
		return 200;
		break;
	case 26:
		return 500;
		break;
	case 27:
		return 1;
		break;
	default:
		return -1;
	}
}

QString CLSSR570::currentUnitHelper() const{
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
