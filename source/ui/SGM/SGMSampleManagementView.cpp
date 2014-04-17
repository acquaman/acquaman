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

	QHBoxLayout *lightsHL = new QHBoxLayout();
	lightsHL->addWidget(lightsOffButton_);
	lightsHL->addWidget(lightsOnButton_);
	lightsHL->addWidget(new QLabel("Illumination %"));
	lightsHL->addWidget(lightLevelSpinBox_);
	lightsHL->addStretch();

	QVBoxLayout *lightTransferVL = new QVBoxLayout();
	lightTransferVL->addLayout(lightsHL);
	lightTransferVL->addLayout(transferMeasureHL);

//	leftVL_->addLayout(lightsHL);
//	leftVL_->addLayout(transferMeasureHL);

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
	if(transferPositionActions_ && transferPositionActions_->isRunning())
		return;
	if(transferPositionActions_)
		delete transferPositionActions_;
	transferPositionActions_ = SGMBeamline::sgm()->createGoToTransferPositionActions();
	transferPositionActions_->start();
}

void SGMSampleManagementView::onMeasurePositionButtonClicked(){
	if(measurementPositionActions_ && measurementPositionActions_->isRunning())
		return;
	if(measurementPositionActions_)
		delete measurementPositionActions_;
	measurementPositionActions_ = SGMBeamline::sgm()->createGoToMeasurementPositionActions();
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
