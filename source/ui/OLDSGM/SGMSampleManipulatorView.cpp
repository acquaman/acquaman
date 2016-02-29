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


#include "SGMSampleManipulatorView.h"
#include "beamline/SGM/SGMBeamline.h"

#include <QLabel>
#include <QPalette>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QSlider>
#include <QIcon>
#include <QGridLayout>
#include <QToolButton>

#include "ui/beamline/AMExtendedControlEditor.h"
#include "beamline/SGM/SGMMAXvMotor.h"

 SGMSampleManipulatorView::~SGMSampleManipulatorView(){}
SGMSampleManipulatorView::SGMSampleManipulatorView(bool bigButtons, QWidget *parent) :
		QGroupBox("SGM SSA Endstation", parent )
{
	lastHVValue_ = 0;
	isJogging_ = false;
	jogStep_ = 0.1;

	upIcon_ = QIcon(":/go-up.png");
	mUpButton_ = new QPushButton(upIcon_, "");
	mUpButton_->setToolTip("Move Up");
	downIcon_ = QIcon(":/go-down.png");
	mDownButton_ = new QPushButton(downIcon_, "");
	mDownButton_->setToolTip("Move Down");
	inboardIcon_ = QIcon(":/go-previous.png");
	mInboardButton_ = new QPushButton(inboardIcon_, "");
	mInboardButton_->setToolTip("Move Inboard");
	outboardIcon_ = QIcon(":/go-next.png");
	mOutboardButton_ = new QPushButton(outboardIcon_, "");
	mOutboardButton_->setToolTip("Move Outboard");
	upstreamIcon_ = QIcon(":/ArrowPlaneOut.png");
	mUpstreamButton_ = new QPushButton(upstreamIcon_, "");
	mUpstreamButton_->setToolTip("Move Upstream");
	downstreamIcon_ = QIcon(":/ArrowPlaneIn.png");
	mDownstreamButton_ = new QPushButton(downstreamIcon_, "");
	mDownstreamButton_->setToolTip("Move Downstream");
	cwIcon_ = QIcon(":/ArrowCW.png");
	mCWButton_ = new QPushButton(cwIcon_, "");
	mCWButton_->setToolTip("Move Clockwise");
	ccwIcon_ = QIcon(":/ArrowCCW.png");
	mCCWButton_ = new QPushButton(ccwIcon_, "");
	mCCWButton_->setToolTip("Move Counter-Clockwise");

	stopAllButton_ = new QPushButton("STOP");
	QPalette sabp = stopAllButton_->palette();
	sabp.setColor(QPalette::ButtonText, Qt::red);
	stopAllButton_->setPalette(sabp);

	jogBox_ = new QCheckBox("Jog");
	jogSettingComboBox_ = new QComboBox();
	jogSettingComboBox_->addItem("0.1 mm");
	jogSettingComboBox_->addItem("0.25 mm");
	jogSettingComboBox_->addItem("0.5 mm");
	jogSettingComboBox_->addItem("1.0 mm");

	transferPositionButton_ = new QPushButton("Transfer Position");
	transferPositionActions_ = 0;//NULL
	measurePositionButton_ = new QPushButton("Measure Position");
	measurementPositionActions_ = 0;//NULL

	mVerticalCtrl_ = SGMBeamline::sgm()->ssaManipulatorZ();
	mHorizontalCtrl_ = SGMBeamline::sgm()->ssaManipulatorX();
	mInPlaneCtrl_ = SGMBeamline::sgm()->ssaManipulatorY();
	mRotationCtrl_ = SGMBeamline::sgm()->ssaManipulatorRot();

	mVerticalNC_ = new AMExtendedControlEditor(mVerticalCtrl_);
	mHorizontalNC_ = new AMExtendedControlEditor(mHorizontalCtrl_);
	mInPlaneNC_ = new AMExtendedControlEditor(mInPlaneCtrl_);
	mRotationNC_ = new AMExtendedControlEditor(mRotationCtrl_);

	illuminatorSlider_ = new QSlider(Qt::Horizontal);
	illuminatorSlider_->setRange(0, 100);
	illuminatorPresets_ = new QButtonGroup();
	illuminatorOff_ = new QToolButton();
	illuminatorOff_->setText("Off");
	illuminatorOff_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	illuminatorDim_ = new QToolButton();
	illuminatorDim_->setText("Dim");
	illuminatorDim_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	illuminatorMid_ = new QToolButton();
	illuminatorMid_->setText("Mid");
	illuminatorMid_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	illuminatorOn_ = new QToolButton();
	illuminatorOn_->setText("On");
	illuminatorOn_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	illuminatorPresets_->addButton(illuminatorOff_, 0);
	illuminatorPresets_->addButton(illuminatorDim_, 1);
	illuminatorPresets_->addButton(illuminatorMid_, 2);
	illuminatorPresets_->addButton(illuminatorOn_, 3);
	QHBoxLayout *ipl = new QHBoxLayout();
	ipl->addWidget(illuminatorOff_);
	ipl->addWidget(illuminatorDim_);
	ipl->addWidget(illuminatorMid_);
	ipl->addWidget(illuminatorOn_);

	if(bigButtons){
		mUpButton_->setMinimumSize(125, 125);
		mDownButton_->setMinimumSize(125, 125);
		mInboardButton_->setMinimumSize(125, 125);
		mOutboardButton_->setMinimumSize(125, 125);
		mUpstreamButton_->setMinimumSize(125, 125);
		mDownstreamButton_->setMinimumSize(125, 125);
		mCWButton_->setMinimumSize(125, 125);
		mCCWButton_->setMinimumSize(125, 125);
		stopAllButton_->setMinimumSize(125, 125);

		transferPositionButton_->setMinimumHeight(100);
		measurePositionButton_->setMinimumHeight(100);

		illuminatorOff_->setMinimumHeight(75);
		illuminatorDim_->setMinimumHeight(75);
		illuminatorMid_->setMinimumHeight(75);
		illuminatorOn_->setMinimumHeight(75);
	}

	connect(mUpButton_, SIGNAL(pressed()), this, SLOT(onMUpButtonPressed()));
	connect(mUpButton_, SIGNAL(released()), this, SLOT(onMUpButtonReleased()));
	connect(mDownButton_, SIGNAL(pressed()), this, SLOT(onMDownButtonPressed()));
	connect(mDownButton_, SIGNAL(released()), this, SLOT(onMDownButtonReleased()));
	connect(mInboardButton_, SIGNAL(pressed()), this, SLOT(onMInboardButtonPressed()));
	connect(mInboardButton_, SIGNAL(released()), this, SLOT(onMInboardButtonReleased()));
	connect(mOutboardButton_, SIGNAL(pressed()), this, SLOT(onMOutboardButtonPressed()));
	connect(mOutboardButton_, SIGNAL(released()), this, SLOT(onMOutboardButtonReleased()));
	connect(mUpstreamButton_, SIGNAL(pressed()), this, SLOT(onMUpstreamButtonPressed()));
	connect(mUpstreamButton_, SIGNAL(released()), this, SLOT(onMUpstreamButtonReleased()));
	connect(mDownstreamButton_, SIGNAL(pressed()), this, SLOT(onMDownstreamButtonPressed()));
	connect(mDownstreamButton_, SIGNAL(released()), this, SLOT(onMDownstreamButtonReleased()));
	connect(mCWButton_, SIGNAL(pressed()), this, SLOT(onMCWButtonPressed()));
	connect(mCWButton_, SIGNAL(released()), this, SLOT(onMCWButtonReleased()));
	connect(mCCWButton_, SIGNAL(pressed()), this, SLOT(onMCCWButtonPressed()));
	connect(mCCWButton_, SIGNAL(released()), this, SLOT(onMCCWButtonReleased()));

	connect(stopAllButton_, SIGNAL(clicked()), this, SLOT(onStopAllButtonClicked()));
	connect(jogBox_, SIGNAL(toggled(bool)), this, SLOT(onJogButtonChecked(bool)));
	connect(jogSettingComboBox_, SIGNAL(activated(int)), this, SLOT(onJogSettingComboBoxChanged(int)));
	connect(transferPositionButton_, SIGNAL(clicked()), this, SLOT(onTransferPositionButtonClicked()));
	connect(measurePositionButton_, SIGNAL(clicked()), this, SLOT(onMeasurePositionButtonClicked()));

	connect(illuminatorSlider_, SIGNAL(sliderMoved(int)), this, SLOT(onIlluminatorSliderValueMoved(int)));
	connect(SGMBeamline::sgm()->ssaIllumination(), SIGNAL(valueChanged(double)), this, SLOT(onIlluminatorFeedbackChanged(double)));
	connect(illuminatorPresets_, SIGNAL(buttonClicked(int)), this, SLOT(onIlluminatorPreset(int)));

	gl_ = new QGridLayout();
	gl_->addWidget(mUpButton_,		0, 2, 1, 1, Qt::AlignCenter);
	gl_->addWidget(mDownButton_,		4, 2, 1, 1, Qt::AlignCenter);
	gl_->addWidget(mInboardButton_,		2, 0, 1, 1, Qt::AlignCenter);
	gl_->addWidget(mOutboardButton_,	2, 4, 1, 1, Qt::AlignCenter);
	gl_->addWidget(mUpstreamButton_,	4, 0, 1, 1, Qt::AlignCenter);
	gl_->addWidget(mDownstreamButton_,	0, 4, 1, 1, Qt::AlignCenter);
	gl_->addWidget(mCWButton_,		0, 0, 1, 1, Qt::AlignCenter);
	gl_->addWidget(mCCWButton_,		4, 4, 1, 1, Qt::AlignCenter);
	gl_->addWidget(stopAllButton_,		2, 2, 1, 1, Qt::AlignCenter);
	gl_->addWidget(jogBox_,			5, 0, 1, 1, Qt::AlignCenter);
	gl_->addWidget(jogSettingComboBox_,	5, 2, 1, 1, Qt::AlignCenter);
	gl_->setRowMinimumHeight(1, 10);
	gl_->setRowMinimumHeight(3, 10);
	gl_->setColumnMinimumWidth(1, 5);
	gl_->setColumnMinimumWidth(3, 5);

	QVBoxLayout *vl1 = new QVBoxLayout();
	vl1->addWidget(mVerticalNC_);
	vl1->addWidget(mHorizontalNC_);
	vl1->addWidget(mInPlaneNC_);
	vl1->addWidget(mRotationNC_);

	QVBoxLayout *vl2 = new QVBoxLayout();
	vl2->addLayout(gl_);
	vl2->addStretch(10);
	if(!bigButtons){
		vl2->addWidget(illuminatorSlider_);
		QLabel *illuminatorLabel = new QLabel("Chamber Light");
		illuminatorLabel->setAlignment(Qt::AlignCenter);
		vl2->addWidget(illuminatorLabel);
		vl2->addLayout(ipl);
		vl2->addWidget(transferPositionButton_);
		vl2->addWidget(measurePositionButton_);
	}
	else{
		vl1->addStretch(10);
		vl1->addWidget(illuminatorSlider_);
		QLabel *illuminatorLabel = new QLabel("Chamber Light");
		illuminatorLabel->setAlignment(Qt::AlignCenter);
		vl1->addWidget(illuminatorLabel);
		vl1->addLayout(ipl);
		vl1->addWidget(transferPositionButton_);
		vl1->addWidget(measurePositionButton_);
	}

	QHBoxLayout *hl = new QHBoxLayout();
	hl->addLayout(vl2);
	hl->addLayout(vl1);
	hl->setContentsMargins(0,0,0,0);
	hl->setSpacing(1);

	gl_->setContentsMargins(10, 10, 5, 10);
	gl_->setSpacing(5);
	vl1->setContentsMargins(5, 10, 10, 10);
	vl1->setSpacing(5);

	setLayout(hl);
}

void SGMSampleManipulatorView::onMUpButtonPressed(){
	if(isJogging_)
		mVerticalCtrl_->move(mVerticalCtrl_->value()+jogStep_);
	else{
		mVerticalCtrl_->setStepVelocity(3000);
		mVerticalCtrl_->setStepAcceleration(2000);
		mVerticalCtrl_->move(152);
	}
}

void SGMSampleManipulatorView::onMUpButtonReleased(){
	if(!isJogging_){
		mVerticalCtrl_->stop();
		mVerticalCtrl_->setStepVelocity(10000);
		mVerticalCtrl_->setStepAcceleration(5000);
	}
}

void SGMSampleManipulatorView::onMDownButtonPressed(){
	if(isJogging_)
		mVerticalCtrl_->move(mVerticalCtrl_->value()-jogStep_);
	else{
		mVerticalCtrl_->setStepVelocity(3000);
		mVerticalCtrl_->setStepAcceleration(2000);
		mVerticalCtrl_->move(-7.5);
	}
}

void SGMSampleManipulatorView::onMDownButtonReleased(){
	if(!isJogging_){
		mVerticalCtrl_->stop();
		mVerticalCtrl_->setStepVelocity(10000);
		mVerticalCtrl_->setStepAcceleration(5000);
	}
}

void SGMSampleManipulatorView::onMInboardButtonPressed(){
	if(isJogging_)
		mHorizontalCtrl_->move(mHorizontalCtrl_->value()-jogStep_);
	else{
		mHorizontalCtrl_->setStepVelocity(10000);
		mHorizontalCtrl_->setStepAcceleration(5000);
		mHorizontalCtrl_->move(-10.0);
	}
}

void SGMSampleManipulatorView::onMInboardButtonReleased(){
	if(!isJogging_){
		mHorizontalCtrl_->stop();
		mHorizontalCtrl_->setStepVelocity(10000);
		mHorizontalCtrl_->setStepAcceleration(5000);
	}
}

void SGMSampleManipulatorView::onMOutboardButtonPressed(){
	if(isJogging_)
		mHorizontalCtrl_->move(mHorizontalCtrl_->value()+jogStep_);
	else{
		mHorizontalCtrl_->setStepVelocity(10000);
		mHorizontalCtrl_->setStepAcceleration(5000);
		mHorizontalCtrl_->move(10.0);
	}
}

void SGMSampleManipulatorView::onMOutboardButtonReleased(){
	if(!isJogging_){
		mHorizontalCtrl_->stop();
		mHorizontalCtrl_->setStepVelocity(10000);
		mHorizontalCtrl_->setStepAcceleration(5000);
	}
}

void SGMSampleManipulatorView::onMUpstreamButtonPressed(){
	if(isJogging_)
		mInPlaneCtrl_->move(mInPlaneCtrl_->value()-jogStep_);
	else{
		mInPlaneCtrl_->setStepVelocity(10000);
		mInPlaneCtrl_->setStepAcceleration(5000);
		mInPlaneCtrl_->move(-10.0);
	}
}

void SGMSampleManipulatorView::onMUpstreamButtonReleased(){
	if(!isJogging_){
		mInPlaneCtrl_->stop();
		mInPlaneCtrl_->setStepVelocity(10000);
		mInPlaneCtrl_->setStepAcceleration(5000);
	}
}

void SGMSampleManipulatorView::onMDownstreamButtonPressed(){
	if(isJogging_)
		mInPlaneCtrl_->move(mInPlaneCtrl_->value()+jogStep_);
	else{
		mInPlaneCtrl_->setStepVelocity(10000);
		mInPlaneCtrl_->setStepAcceleration(5000);
		mInPlaneCtrl_->move(10.0);
	}
}

void SGMSampleManipulatorView::onMDownstreamButtonReleased(){
	if(!isJogging_){
		mInPlaneCtrl_->stop();
		mInPlaneCtrl_->setStepVelocity(10000);
		mInPlaneCtrl_->setStepAcceleration(5000);
	}
}

void SGMSampleManipulatorView::onMCWButtonPressed(){
	if(isJogging_)
		mRotationCtrl_->move(mRotationCtrl_->value()-jogStep_);
	else{
		mRotationCtrl_->setStepVelocity(3000);
		mRotationCtrl_->setStepAcceleration(2000);
		mRotationCtrl_->move(-360.0);
	}
}

void SGMSampleManipulatorView::onMCWButtonReleased(){
	if(!isJogging_){
		mRotationCtrl_->stop();
		mRotationCtrl_->setStepVelocity(10000);
		mRotationCtrl_->setStepAcceleration(5000);
	}
}

void SGMSampleManipulatorView::onMCCWButtonPressed(){
	if(isJogging_)
		mRotationCtrl_->move(mRotationCtrl_->value()+jogStep_);
	else{
		mRotationCtrl_->setStepVelocity(3000);
		mRotationCtrl_->setStepAcceleration(2000);
		mRotationCtrl_->move(360.0);
	}
}

void SGMSampleManipulatorView::onMCCWButtonReleased(){
	if(!isJogging_){
		mRotationCtrl_->stop();
		mRotationCtrl_->setStepVelocity(10000);
		mRotationCtrl_->setStepAcceleration(5000);
	}
}

void SGMSampleManipulatorView::onStopAllButtonClicked(){
	mVerticalCtrl_->stop();
	mHorizontalCtrl_->stop();
	mInPlaneCtrl_->stop();
	mRotationCtrl_->stop();
}

void SGMSampleManipulatorView::onJogButtonChecked(bool checked){
	isJogging_ = checked;
}

void SGMSampleManipulatorView::onJogSettingComboBoxChanged(int index){
	switch(index){
	case 0:
		jogStep_ = 0.1;
		break;
	case 1:
		jogStep_ = 0.25;
		break;
	case 2:
		jogStep_ = 0.5;
		break;
	case 3:
		jogStep_ = 1.0;
		break;
	}
}

void SGMSampleManipulatorView::onTransferPositionButtonClicked(){
	if(transferPositionActions_ && !transferPositionActions_->inFinalState())
		return;
	if(transferPositionActions_)
		transferPositionActions_->deleteLater();
	transferPositionActions_ = SGMBeamline::sgm()->createGoToTransferPositionActions3();
	transferPositionActions_->start();
}

void SGMSampleManipulatorView::onMeasurePositionButtonClicked(){
	if(measurementPositionActions_ && !measurementPositionActions_->inFinalState())
		return;
	if(measurementPositionActions_)
		measurementPositionActions_->deleteLater();
	measurementPositionActions_ = SGMBeamline::sgm()->createGoToMeasurementPositionActions3();
	measurementPositionActions_->start();
}

void SGMSampleManipulatorView::onIlluminatorSliderValueMoved(int newValue){
	SGMBeamline::sgm()->ssaIllumination()->move(newValue);
}

void SGMSampleManipulatorView::onIlluminatorFeedbackChanged(double newValue){
	if(!illuminatorSlider_->isSliderDown())
		illuminatorSlider_->setValue(newValue);
}

void SGMSampleManipulatorView::onIlluminatorPreset(int presetIndex){
	switch(presetIndex){
	case 0:
		SGMBeamline::sgm()->ssaIllumination()->move(0);
		break;
	case 1:
		SGMBeamline::sgm()->ssaIllumination()->move(10);
		break;
	case 2:
		SGMBeamline::sgm()->ssaIllumination()->move(25);
		break;
	case 3:
		SGMBeamline::sgm()->ssaIllumination()->move(100);
		break;
	}
}
