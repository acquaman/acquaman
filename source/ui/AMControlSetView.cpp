/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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
AMControlSetView::AMControlSetView(AMControlSet *viewSet, bool configureOnly, QWidget *parent) :
		QGroupBox(parent)
{
	viewSet_ = viewSet;
	configureOnly_ = configureOnly;
	setTitle(viewSet->name());
	QVBoxLayout *vl = new QVBoxLayout();
	AMControlEditor *tmpCE;
	AMControl *tmpCtrl;
	for(int x = 0; x < viewSet_->count(); x++){
		tmpCtrl = viewSet_->at(x);
		tmpCE = new AMControlEditor(tmpCtrl, 0, false, configureOnly_);
		vl->addWidget(tmpCE);
		controlBoxes_.append(tmpCE);
		connect(tmpCE, SIGNAL(setpointRequested(double)), this, SLOT(onConfigurationValueChanged()));
	}

	hl_ = new QHBoxLayout(this);
	hl_->addLayout(vl);
	setLayout(hl_);
	setFixedSize(300, 200);

	connect(viewSet_, SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SLOT(onControlSetValuesChanged(AMControlInfoList)));
}

AMControlInfoList AMControlSetView::currentValues(){
	return viewSet_->toInfoList();
}

AMControlInfoList AMControlSetView::configValues(){
	AMControlInfoList rv;

	if(!configureOnly_)
		return currentValues();

	int numControls = viewSet_->count();
	for(int i=0; i<numControls; i++) {
		AMControl* c = boxAt(i)->control();
		rv.append( AMControlInfo(c->name(), boxAt(i)->setpoint(), c->minimumValue(), c->maximumValue(), c->units(), c->tolerance(), c->description()) );
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

void AMControlSetView::onControlSetValuesChanged(AMControlInfoList infoList){
	if(!configureOnly_)
		emit currentValuesChanged(infoList);
}

void AMControlSetView::onConfigurationValueChanged(){
	if(configureOnly_)
		emit configValuesChanged(configValues());
}
