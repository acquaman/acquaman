#include "SGMSampleManagementView.h"

#include <QBoxLayout>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QLabel>

#include "beamline/SGM/SGMBeamline.h"
#include "ui/SGM/SGMSampleManipulatorMotorGroupView.h"

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

void SGMSampleManagementView::onTransferPositionButtonClicked(){
	if(transferPositionActions_ && !transferPositionActions_->inFinalState())
		return;
	if(transferPositionActions_)
		transferPositionActions_->deleteLater();
	transferPositionActions_ = SGMBeamline::sgm()->createGoToTransferPositionActions3();
	transferPositionActions_->start();
}

void SGMSampleManagementView::onMeasurePositionButtonClicked(){
	if(measurementPositionActions_ && !measurementPositionActions_->inFinalState())
		return;
	if(measurementPositionActions_)
		measurementPositionActions_->deleteLater();
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
