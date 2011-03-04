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


#include "AMDetectorView.h"

AMDetectorView::AMDetectorView(QWidget *parent) :
	QGroupBox(parent)
{
}

PGTDetectorView::PGTDetectorView(PGTDetector *detector, AMDetectorInfo *configDetector, bool editMode, QWidget *parent) :
		PGTDetectorInfoView(detector, configDetector, true, parent)
{
	detector_ = detector;
	editMode_ = editMode;
	integrationTimeFbk_ = NULL;
	integrationModeFbk_ = NULL;
	hvFbk_ = NULL;
	QPair<double, double> tmpRange;
	tmpRange = detector_->integrationTimeCtrl()->range();
	integrationTimeBox_->setRange(tmpRange.first, tmpRange.second);
	integrationTimeBox_->setValue(detector_->integrationTimeCtrl()->value());
	integrationModeBox_->clear();
	integrationModeBox_->addItems(detector_->integrationModeCtrl()->enumNames());
	integrationModeBox_->setCurrentIndex( (int)detector_->integrationModeCtrl()->value() );
	tmpRange = detector_->hvSetpointCtrl()->range();
	hvSetpointBox_->setRange(tmpRange.first, tmpRange.second);
	hvSetpointBox_->setValue(detector_->hvSetpointCtrl()->value());
	switchToEditBox_->setEnabled(true);
	connect(switchToEditBox_, SIGNAL(clicked()), this, SLOT(setEditable()));
}

void PGTDetectorView::onIntegrationModeUpdate(double value){
	integrationModeFbk_->setCurrentIndex((int)value);
}

void PGTDetectorView::onIntegrationModeChange(int index){
	detector_->integrationModeCtrl()->move(index);
}

void PGTDetectorView::setEditMode(bool editMode){
	editMode_ = editMode;
	QHBoxLayout *tmpHB;
	QPair<double, double> tmpRange;
	if(editMode_){
		switchToEditBox_->setText("Switch to Configure Mode");
		if(!integrationTimeFbk_){
			integrationTimeFbk_ = new QDoubleSpinBox();
			integrationTimeFbk_->setEnabled(false);
			tmpRange = detector_->integrationTimeRange();
			integrationTimeFbk_->setRange(tmpRange.first, tmpRange.second);
			integrationTimeFbk_->setValue(detector_->integrationTimeCtrl()->value());
			connect(detector_->integrationTimeCtrl(), SIGNAL(valueChanged(double)), integrationTimeFbk_, SLOT(setValue(double)));
		}
		if(!integrationModeFbk_){
			integrationModeFbk_ = new QComboBox();
			integrationModeFbk_->setEnabled(false);
			integrationModeFbk_->addItems(detector_->integrationModeList());
			integrationModeFbk_->setCurrentIndex((int)detector_->integrationModeCtrl()->value());
			connect(detector_->integrationModeCtrl(), SIGNAL(valueChanged(double)), this, SLOT(onIntegrationModeUpdate(double)));
		}
		if(!hvFbk_){
			hvFbk_ = new QDoubleSpinBox();
			hvFbk_->setEnabled(false);
			tmpRange = detector_->hvSetpointRange();
			hvFbk_->setRange(tmpRange.first, tmpRange.second);
			hvFbk_->setValue(detector_->hvFbkCtrl()->value());
			connect(detector_->hvFbkCtrl(), SIGNAL(valueChanged(double)), hvFbk_, SLOT(setValue(double)));
		}
		connect(integrationTimeBox_, SIGNAL(valueChanged(double)), detector_->integrationTimeCtrl(), SLOT(move(double)));
		connect(integrationModeBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onIntegrationModeChange(int)));
		connect(hvSetpointBox_, SIGNAL(valueChanged(double)), detector_->hvSetpointCtrl(), SLOT(move(double)));
		tmpHB = (QHBoxLayout*)fl_->itemAt(0, QFormLayout::FieldRole);
		tmpHB->addWidget(integrationTimeFbk_);
		integrationTimeFbk_->show();
		tmpHB = (QHBoxLayout*)fl_->itemAt(1, QFormLayout::FieldRole);
		tmpHB->addWidget(integrationModeFbk_);
		integrationModeFbk_->show();
		tmpHB = (QHBoxLayout*)fl_->itemAt(2, QFormLayout::FieldRole);
		tmpHB->addWidget(hvFbk_);
		hvFbk_->show();
		resize(width()+hvSetpointBox_->width(), height());
		if( !detector_->settingsMatchFbk((PGTDetectorInfo*)writeDetectorInfo_) ){
			int ret = QMessageBox::question(this, "PGT Silicon Drift Detector", "Configuration settings are different from actual settings.\n"
											"Do you wish to write the changes to the detector?",
											QMessageBox::Yes,
											QMessageBox::No);
			if(ret != QMessageBox::Yes){
				((PGTDetectorInfo*)writeDetectorInfo_)->setIntegrationTime(detector_->integrationTimeCtrl()->value());
				integrationTimeBox_->setValue( ((PGTDetectorInfo*)writeDetectorInfo_)->integrationTime() );
				((PGTDetectorInfo*)writeDetectorInfo_)->setIntegrationMode(detector_->integrationModeList().at(detector_->integrationModeCtrl()->value()));
				integrationModeBox_->setCurrentIndex( ((PGTDetectorInfo*)writeDetectorInfo_)->integrationModeList().indexOf(((PGTDetectorInfo*)writeDetectorInfo_)->integrationMode()) );
				((PGTDetectorInfo*)writeDetectorInfo_)->setHVSetpoint(detector_->hvSetpointCtrl()->value());
				hvSetpointBox_->setValue( ((PGTDetectorInfo*)writeDetectorInfo_)->hvSetpoint() );
			}
			else
				detector_->setControls((PGTDetectorInfo*)writeDetectorInfo_);
		}
	}
	else{
		disconnect(integrationTimeBox_, SIGNAL(valueChanged(double)), detector_->integrationTimeCtrl(), SLOT(move(double)));
		disconnect(integrationModeBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onIntegrationModeChange(int)));
		disconnect(hvSetpointBox_, SIGNAL(valueChanged(double)), detector_->hvSetpointCtrl(), SLOT(move(double)));
		switchToEditBox_->setText("Switch to Edit Mode");
		tmpHB = (QHBoxLayout*)fl_->itemAt(0, QFormLayout::FieldRole);
		tmpHB->removeWidget(integrationTimeFbk_);
		integrationTimeFbk_->hide();
		tmpHB = (QHBoxLayout*)fl_->itemAt(1, QFormLayout::FieldRole);
		tmpHB->removeWidget(integrationModeFbk_);
		integrationModeFbk_->hide();
		tmpHB = (QHBoxLayout*)fl_->itemAt(2, QFormLayout::FieldRole);
		tmpHB->removeWidget(hvFbk_);
		hvFbk_->hide();
		resize(width()-hvFbk_->width(), height());
	}
}

void PGTDetectorView::setEditable(){
	setEditMode(!editMode_);
}

MCPDetectorView::MCPDetectorView(MCPDetector *detector, AMDetectorInfo *configDetector, bool editMode, QWidget *parent) :
		MCPDetectorInfoView(detector, configDetector, true, parent)
{
	detector_ = detector;
	editMode_ = editMode;
	hvFbk_ = NULL;
	QPair<double, double> tmpRange;
	tmpRange = detector_->hvSetpointCtrl()->range();
	hvSetpointBox_->setRange(tmpRange.first, tmpRange.second);
	hvSetpointBox_->setValue(detector_->hvSetpointCtrl()->value());
	switchToEditBox_->setEnabled(true);
	connect(switchToEditBox_, SIGNAL(clicked()), this, SLOT(setEditable()));
}

void MCPDetectorView::setEditMode(bool editMode){
	editMode_ = editMode;
	QHBoxLayout *tmpHB;
	QPair<double, double> tmpRange;
	if(editMode_){
		switchToEditBox_->setText("Switch to Configure Mode");
		if(!hvFbk_){
			hvFbk_ = new QDoubleSpinBox();
			hvFbk_->setEnabled(false);
			tmpRange = detector_->hvSetpointRange();
			hvFbk_->setRange(tmpRange.first, tmpRange.second);
			hvFbk_->setValue(detector_->hvFbkCtrl()->value());
			connect(detector_->hvFbkCtrl(), SIGNAL(valueChanged(double)), hvFbk_, SLOT(setValue(double)));
		}
		tmpHB = (QHBoxLayout*)fl_->itemAt(0, QFormLayout::FieldRole);
		tmpHB->addWidget(hvFbk_);
		connect(hvSetpointBox_, SIGNAL(valueChanged(double)), detector_->hvSetpointCtrl(), SLOT(move(double)));
		hvFbk_->show();
		resize(width()+hvSetpointBox_->width(), height());
		if( !detector_->settingsMatchFbk((MCPDetectorInfo*)writeDetectorInfo_) ){
			int ret = QMessageBox::question(this, "MCP Detector", "Configuration settings are different from actual settings.\n"
											"Do you wish to write the changes to the detector?",
											QMessageBox::Yes,
											QMessageBox::No);
			if(ret != QMessageBox::Yes){
				((MCPDetectorInfo*)writeDetectorInfo_)->setHVSetpoint(detector_->hvSetpointCtrl()->value());
				hvSetpointBox_->setValue( ((MCPDetectorInfo*)writeDetectorInfo_)->hvSetpoint() );
			}
			else
				detector_->setControls((MCPDetectorInfo*)writeDetectorInfo_);
		}
	}
	else{
		disconnect(hvSetpointBox_, SIGNAL(valueChanged(double)), detector_->hvSetpointCtrl(), SLOT(move(double)));
		switchToEditBox_->setText("Switch to Edit Mode");
		tmpHB = (QHBoxLayout*)fl_->itemAt(0, QFormLayout::FieldRole);
		tmpHB->removeWidget(hvFbk_);
		hvFbk_->hide();
		resize(width()-hvFbk_->width(), height());
	}
}

void MCPDetectorView::setEditable(){
	setEditMode(!editMode_);
}

AMDetectorSetView::AMDetectorSetView(AMOldDetectorInfoSet *viewSet, AMOldDetectorInfoSet *configSet, bool setup, QWidget *parent) :
		AMDetectorInfoSetView(viewSet, configSet, false, parent)
{
	editMode_ = true;
	if(setup)
		AMDetectorInfoSetView::runSetup();
}

void AMDetectorSetView::setEditMode(bool editMode){
	editMode_ = editMode;
}

void AMDetectorSetView::setEditable(){
	editMode_ = true;
}

void AMDetectorSetView::runSetup(){
	runSetup();
}

QWidget* AMDetectorSetView::detailViewByType(AMDetectorInfo *detector, AMDetectorInfo *configDetector){

	if(!editMode_)
		return AMDetectorInfoSetView::detailViewByType(detector, configDetector);
	#warning "D: same edit to review:"

	else if( qobject_cast<PGTDetector*>(detector) ){
		return new PGTDetectorView( qobject_cast<PGTDetector*>(detector), configDetector);
	}
	else if( qobject_cast<MCPDetector*>(detector) )
		return new MCPDetectorView( qobject_cast<MCPDetector*>(detector), configDetector);

	else
		return new QGroupBox();
}
