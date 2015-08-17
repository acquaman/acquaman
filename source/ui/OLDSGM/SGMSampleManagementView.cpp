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


#include "SGMSampleManagementView.h"
#include "ui/AMMotorGroupView.h"

#include <QBoxLayout>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QLabel>

#include "beamline/SGM/SGMBeamline.h"
SGMSampleManagementView::SGMSampleManagementView(QWidget *parent) :
	AMBeamlineSampleManagementView(SGMBeamline::sgm(), SGMBeamline::sgm()->motorGroup(), parent)
{

	transferPositionButton_ = new QPushButton("Transfer Position");
	transferPositionActions_ = 0;//NULL
	measurePositionButton_ = new QPushButton("Measure Position");
	measurementPositionActions_ = 0;//NULL

	QVBoxLayout *transferMeasureVL = new QVBoxLayout();
	transferMeasureVL->addWidget(transferPositionButton_);
	transferMeasureVL->addWidget(measurePositionButton_);

	QHBoxLayout *transferMeasureHL = new QHBoxLayout();
	transferMeasureHL->addLayout(transferMeasureVL);
	transferMeasureHL->addStretch();

	lightsOffButton_ = new QPushButton("Off");
	lightsOnButton_ = new QPushButton("On");
	lightLevelSpinBox_ = new QDoubleSpinBox();
	lightLevelSpinBox_->setRange(0, 100);

	QHBoxLayout *lightsTopHL = new QHBoxLayout();
	lightsTopHL->addWidget(lightsOffButton_);
	lightsTopHL->addWidget(lightsOnButton_);
	lightsTopHL->addStretch();
	QHBoxLayout *lightsBottomHL = new QHBoxLayout();
	lightsBottomHL->addWidget(new QLabel("Illumination %"));
	lightsBottomHL->addWidget(lightLevelSpinBox_);
	lightsBottomHL->addStretch();

	QVBoxLayout *lightTransferVL = new QVBoxLayout();
	lightTransferVL->addLayout(lightsTopHL);
	lightTransferVL->addLayout(lightsBottomHL);
	lightTransferVL->addLayout(transferMeasureHL);
	lightTransferVL->addStretch();

	leftVL_->removeWidget(motorGroupView_);

	QHBoxLayout *lowerLeftHL = new QHBoxLayout();
	lowerLeftHL->addWidget(motorGroupView_);
	lowerLeftHL->addLayout(lightTransferVL);
	leftVL_->addLayout(lowerLeftHL);

	connect(transferPositionButton_, SIGNAL(clicked()), this, SLOT(onTransferPositionButtonClicked()));
	connect(measurePositionButton_, SIGNAL(clicked()), this, SLOT(onMeasurePositionButtonClicked()));

	connect(lightsOffButton_, SIGNAL(clicked()), this, SLOT(onLightsOffButtonClicked()));
	connect(lightsOnButton_, SIGNAL(clicked()), this, SLOT(onLightsOnButtonClicked()));
	connect(lightLevelSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(onLightLevelSpinBoxValueChanged(double)));
	connect(SGMBeamline::sgm()->ssaIllumination(), SIGNAL(valueChanged(double)), this, SLOT(onLightLevelControlValueChanged(double)));
	if(SGMBeamline::sgm()->ssaIllumination()->isConnected())
		onLightLevelControlValueChanged(SGMBeamline::sgm()->ssaIllumination()->value());
}

SGMSampleManagementView::~SGMSampleManagementView()
{
}

void SGMSampleManagementView::onTransferPositionButtonClicked(){
	if(transferPositionActions_ && !transferPositionActions_->inFinalState())
		return;
	if(transferPositionActions_)
		transferPositionActions_->deleteLater();
	SGMBeamline::sgm()->setMoveInFocus(false);
	transferPositionActions_ = SGMBeamline::sgm()->createGoToTransferPositionActions3();
	transferPositionActions_->start();
}

void SGMSampleManagementView::onMeasurePositionButtonClicked(){
	if(measurementPositionActions_ && !measurementPositionActions_->inFinalState())
		return;
	if(measurementPositionActions_)
		measurementPositionActions_->deleteLater();
	SGMBeamline::sgm()->setMoveInFocus(false);
	measurementPositionActions_ = SGMBeamline::sgm()->createGoToMeasurementPositionActions3();
	measurementPositionActions_->start();
}

void SGMSampleManagementView::onLightsOffButtonClicked(){
	SGMBeamline::sgm()->ssaIllumination()->move(0);
}

void SGMSampleManagementView::onLightsOnButtonClicked(){
	SGMBeamline::sgm()->ssaIllumination()->move(100);
}

void SGMSampleManagementView::onLightLevelSpinBoxValueChanged(double value){
	SGMBeamline::sgm()->ssaIllumination()->move(value);
}

void SGMSampleManagementView::onLightLevelControlValueChanged(double value){
	lightLevelSpinBox_->blockSignals(true);
	lightLevelSpinBox_->setValue(value);
	lightLevelSpinBox_->blockSignals(false);
}
