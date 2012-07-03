#include "AMControlButton.h"

#include "beamline/AMControl.h"

AMControlButton::AMControlButton(AMControl *control, QWidget *parent) :
		QToolButton(parent)
{
	setObjectName("AMControlButton");

	control_ = control;
	downValue_ = 1;
	upValue_ = 0;
	programaticToggle_ = false;
	if(control_){
		if(control_->description() != "")
			setText(control_->description());
		else
			setText(control_->name());
	}
	setHappy(false);

	// Make connections:
	if(control_) {
		connect(control_, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
		connect(control_, SIGNAL(connected(bool)), this, SLOT(setHappy(bool)));
	}
	connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));
	connect(this, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)));

	if(control_ && control_->isConnected()){
		programaticToggle_ = true;
		onValueChanged(control_->value());
		setHappy(control_->isConnected());
	}
}

void AMControlButton::setDownValue(double downValue){
	downValue_ = downValue;
}

void AMControlButton::setUpValue(double upValue){
	upValue_ = upValue;
}

void AMControlButton::overrideText(const QString &text){
	setText(text);
}

void AMControlButton::setCheckable(bool checkable){
	QToolButton::setCheckable(checkable);
	onValueChanged(control_->value());
}

void AMControlButton::onValueChanged(double newVal) {
	Q_UNUSED(newVal)
	if(isCheckable()){
		if(control_->value() == downValue_)
			setChecked(true);
		else if(control_->value() == upValue_)
			setChecked(false);
	}
}

void AMControlButton::onClicked(){
	if(isCheckable())
		return;
	control_->move(downValue_);
}

void AMControlButton::onToggled(bool toggled){
	if(programaticToggle_){
		programaticToggle_ = false;
		return;
	}
	if(toggled && !control_->withinTolerance(downValue_))
		control_->move(downValue_);
	else if(!toggled && !control_->withinTolerance(upValue_))
		control_->move(upValue_);
}

void AMControlButton::setHappy(bool happy) {
	Q_UNUSED(happy)
}

