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


#include "AMControlSetView.h"

/// Sets the title of the group box based on the name() function of the AMControlSet.
/// Loops through the list of AMControls in the AMControlSet and create an appropriate spinbox.
/// Adds the spin box and a label (from the AMControl objectName() function) and add to an internal form layout.
 AMControlSetView::~AMControlSetView(){}
AMControlSetView::AMControlSetView(AMControlSet *viewSet, bool configureOnly, QWidget *parent) :
		QGroupBox(parent)
{
	viewSet_ = viewSet;
	configureOnly_ = configureOnly;
	setTitle(viewSet->name());
	QVBoxLayout *vl = new QVBoxLayout();
	AMExtendedControlEditor *tmpCE;
	AMControl *tmpCtrl;
	for(int x = 0; x < viewSet_->count(); x++){
		tmpCtrl = viewSet_->at(x);
		tmpCE = new AMExtendedControlEditor(tmpCtrl, 0, false, configureOnly_);
		vl->addWidget(tmpCE);
		controlBoxes_.append(tmpCE);
		connect(tmpCE, SIGNAL(setpointRequested(double)), this, SLOT(onConfigurationValueChanged()));
	}

	hl_ = new QHBoxLayout();
	hl_->addLayout(vl);
	setLayout(hl_);
	setFixedSize(300, 200);

	connect(viewSet_, SIGNAL(controlSetValuesChanged()), this, SLOT(onControlSetValuesChanged()));
}

AMControlInfoList AMControlSetView::currentValues(){
	return viewSet_->toInfoList();
}

AMControlInfoList AMControlSetView::configValues(){
	AMControlInfoList rv;


	int numControls = viewSet_->count();
	for(int i=0; i<numControls; i++) {
		AMControlInfo info = boxAt(i)->control()->toInfo();
		info.setValue(boxAt(i)->setpoint());
		qDebug() << "At " << i << "box setpoint is " << boxAt(i)->setpoint();
		rv.append(info);
	}

	return rv;
}

void AMControlSetView::setFromInfoList(const AMControlInfoList &infoList){
	if(!configureOnly_){
		viewSet_->setFromInfoList(infoList);
		return;
	}

	int numControls = viewSet_->count();
	for(int x = 0; x < numControls; x++)
		if(boxAt(x)->setpoint() != infoList.at(x).value())
			boxAt(x)->setSetpoint(infoList.at(x).value());
}

void AMControlSetView::setDisabled(bool disabled){
	for(int x = controlBoxes_.count()-1; x >= 0; x--)
		controlBoxes_[x]->setReadOnlyPreference(disabled);
}

void AMControlSetView::onControlSetValuesChanged(){
	if(!configureOnly_)
		emit currentValuesChanged(viewSet_->toInfoList());
}

void AMControlSetView::onConfigurationValueChanged(){
	if(configureOnly_){
		qDebug() << "Config values are ";
		for(int x = 0, size = configValues().count(); x < size; x++)
			qDebug() << configValues().at(x).name() << configValues().at(x).value();
		emit configValuesChanged(configValues());
	}
}
