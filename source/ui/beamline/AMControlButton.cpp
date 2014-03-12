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


#include "AMControlButton.h"

#include "beamline/AMControl.h"

 AMControlButton::~AMControlButton(){}
AMControlButton::AMControlButton(AMControl *control, QWidget *parent) :
		QToolButton(parent)
{
	setObjectName("AMControlButton");

	control_ = control;
	wasConnected_ = false;
	downValue_ = 1;
	upValue_ = 0;
	programaticToggle_ = false;
	if(control_){
		if(control_->description() != "")
			setText(control_->description());
		else
			setText(control_->name());
	}

	// Make connections:
	if(control_) {
		connect(control_, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
		connect(control_, SIGNAL(connected(bool)), this, SLOT(onConnected(bool)));
	}
	connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));
	connect(this, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)));

	if(control_ && control_->isConnected())
		onConnected(control_->isConnected());

	if(isCheckable()){
		QPalette newPalette = palette();
		QColor newTextColor = QColor(Qt::red);
		newPalette.setColor(QPalette::WindowText, newTextColor);
		newPalette.setColor(QPalette::ButtonText, newTextColor);
		setPalette(newPalette);
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
		QPalette newPalette = palette();
		QColor newTextColor;
		if(control_->value() == downValue_){
			setChecked(true);
			newTextColor = QColor(Qt::black);
		}
		else if(control_->value() == upValue_){
			setChecked(false);
			newTextColor = QColor(Qt::red);
		}
		newPalette.setColor(QPalette::WindowText, newTextColor);
		newPalette.setColor(QPalette::ButtonText, newTextColor);
		setPalette(newPalette);
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

void AMControlButton::onConnected(bool connected){
	if(!wasConnected_ && connected){
		if(control_->withinTolerance(downValue_)){
			programaticToggle_ = true;
			onValueChanged(control_->value());
		}
		wasConnected_ = true;
	}
}

