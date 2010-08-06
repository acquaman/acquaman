#include "AMDetectorView.h"

AMDetectorView::AMDetectorView(QWidget *parent) :
	QGroupBox(parent)
{
}

PGTDetectorView::PGTDetectorView(PGTDetector *detector, bool editMode, QWidget *parent) :
		PGTDetectorInfoView(detector, true, parent)
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

void PGTDetectorView::setEditMode(bool editMode){
	editMode_ = editMode;
	QHBoxLayout *tmpHB;
	if(editMode_){
		switchToEditBox_->setText("Switch to Configure Mode");
		if(!integrationTimeFbk_){
			integrationTimeFbk_ = new QDoubleSpinBox();
			integrationTimeFbk_->setEnabled(false);
			integrationTimeFbk_->setValue(detector_->integrationTimeCtrl()->value());
		}
		if(!integrationModeFbk_){
			integrationModeFbk_ = new QComboBox();
			integrationModeFbk_->setEnabled(false);
//			integrationModeFbk_->addItems(detector_->integrationModes());
			integrationModeFbk_->addItems(detector_->integrationModeList());
			integrationModeFbk_->setCurrentIndex((int)detector_->integrationModeCtrl()->value());
		}
		if(!hvFbk_){
			hvFbk_ = new QDoubleSpinBox();
			hvFbk_->setValue(detector_->hvFbkCtrl()->value());
			hvFbk_->setEnabled(false);
		}
		tmpHB = (QHBoxLayout*)fl_->itemAt(0, QFormLayout::FieldRole);
		tmpHB->addWidget(integrationTimeFbk_);
		integrationTimeFbk_->show();
		tmpHB = (QHBoxLayout*)fl_->itemAt(1, QFormLayout::FieldRole);
		tmpHB->addWidget(integrationModeFbk_);
		integrationModeFbk_->show();
		tmpHB = (QHBoxLayout*)fl_->itemAt(2, QFormLayout::FieldRole);
		tmpHB->addWidget(hvFbk_);
		hvFbk_->show();
		connect(detector_->integrationTimeCtrl(), SIGNAL(valueChanged(double)), integrationTimeFbk_, SLOT(setValue(double)));
		connect(detector_->integrationModeCtrl(), SIGNAL(valueChanged(double)), this, SLOT(onIntegrationModeUpdate(double)));
		connect(detector_->hvFbkCtrl(), SIGNAL(valueChanged(double)), hvFbk_, SLOT(setValue(double)));
		resize(width()+hvSetpointBox_->width(), height());
	}
	else{
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

MCPDetectorView::MCPDetectorView(MCPDetector *detector, bool editMode, QWidget *parent) :
		MCPDetectorInfoView(detector, true, parent)
{
	detector_ = detector;
	editMode_ = editMode;
	hvFbk_ = NULL;
	switchToEditBox_->setEnabled(true);
	connect(switchToEditBox_, SIGNAL(clicked()), this, SLOT(setEditable()));
}

void MCPDetectorView::setEditMode(bool editMode){
	editMode_ = editMode;
	QHBoxLayout *tmpHB;
	if(editMode_){
		switchToEditBox_->setText("Switch to Configure Mode");
		if(!hvFbk_){
			hvFbk_ = new QDoubleSpinBox();
			hvFbk_->setValue(detector_->hvFbkCtrl()->value());
			hvFbk_->setEnabled(false);
		}
		tmpHB = (QHBoxLayout*)fl_->itemAt(0, QFormLayout::FieldRole);
		tmpHB->addWidget(hvFbk_);
		hvFbk_->show();
		connect(detector_->hvFbkCtrl(), SIGNAL(valueChanged(double)), hvFbk_, SLOT(setValue(double)));
		resize(width()+hvSetpointBox_->width(), height());
	}
	else{
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

AMDetectorSetView::AMDetectorSetView(AMDetectorInfoSet *viewSet, bool setup, QWidget *parent) :
		AMDetectorInfoSetView(viewSet, false, parent)
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

QWidget* AMDetectorSetView::detailViewByType(AMDetectorInfo *detector){
	if(!editMode_)
		return AMDetectorInfoSetView::detailViewByType(detector);
	else if(detector->typeDescription() == "PGT SDD Spectrum-Output Detector"){
		return new PGTDetectorView( (PGTDetector*)detector);
	}
	else if(detector->typeDescription() == "MCP Detector")
		return new MCPDetectorView( (MCPDetector*)detector);
	else
		return new QGroupBox();
}
