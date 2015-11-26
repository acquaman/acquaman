#include "SGMEnergyTrajectoryView.h"
#include "beamline/SGM/energy/SGMEnergyControlSet.h"

#include <QGridLayout>
#include <QLabel>
SGMEnergyTrajectoryView::SGMEnergyTrajectoryView(SGMEnergyControlSet* controlSet,
                                                 QWidget *parent) :
    QWidget(parent)
{
	controlSet_ = controlSet;
	connectedOnce_ = false;

	if(controlSet_->isConnected()) {
		onControlsConnected(true);
	} else {

		connect(controlSet_, SIGNAL(connected(bool)),
		        this, SLOT(onControlsConnected(bool)));
	}
}

void SGMEnergyTrajectoryView::onStartPushButtonClicked()
{
	controlSet_->energyTrajectoryTarget()->move(targetSpinBox_->value());
	controlSet_->energyTrajectoryTime()->move(timeSpinBox_->value());

	controlSet_->energyTrajectoryStart()->move(1);
}

void SGMEnergyTrajectoryView::onControlsConnected(bool isConnected)
{
	if(isConnected && !connectedOnce_) {

		connectedOnce_ = true;

		setupUi();
		setupData();
		setupConnections();
	}
}

void SGMEnergyTrajectoryView::setupUi()
{
	QGridLayout* gridLayout = new QGridLayout();
	setLayout(gridLayout);

	targetSpinBox_ = new QDoubleSpinBox();
	gridLayout->addWidget(new QLabel("Target"), 0, 0);
	gridLayout->addWidget(targetSpinBox_, 0, 1);

	timeSpinBox_ = new QDoubleSpinBox();
	gridLayout->addWidget(new QLabel("Time"), 1, 0);
	gridLayout->addWidget(timeSpinBox_, 1, 1);

	startButton_ = new QPushButton("Start");
	gridLayout->addWidget(startButton_, 2, 0, 1, 2);
}

void SGMEnergyTrajectoryView::setupData()
{

	targetSpinBox_->setRange(controlSet_->energyTrajectoryTarget()->minimumValue(),
				   controlSet_->energyTrajectoryTarget()->maximumValue());
	targetSpinBox_->setSuffix(controlSet_->energyTrajectoryTarget()->units());

	timeSpinBox_->setRange(controlSet_->energyTrajectoryTime()->minimumValue(),
	                       controlSet_->energyTrajectoryTime()->maximumValue());
	timeSpinBox_->setSuffix(controlSet_->energyTrajectoryTime()->units());



}

void SGMEnergyTrajectoryView::setupConnections()
{
	connect(startButton_, SIGNAL(clicked()),
	        this, SLOT(onStartPushButtonClicked()));
}



