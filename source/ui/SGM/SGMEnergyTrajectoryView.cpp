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
	controlSet_->energyTrajectoryStartpoint()->move(startpointSpinBox_->value());
	controlSet_->energyTrajectoryEndpoint()->move(endpointSpinBox_->value());
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

	startpointSpinBox_ = new QDoubleSpinBox();
	gridLayout->addWidget(new QLabel("Startpoint"), 0, 0);
	gridLayout->addWidget(startpointSpinBox_, 0, 1);

	endpointSpinBox_ = new QDoubleSpinBox();
	gridLayout->addWidget(new QLabel("Endpoint"), 1, 0);
	gridLayout->addWidget(endpointSpinBox_, 1, 1);

	timeSpinBox_ = new QDoubleSpinBox();
	gridLayout->addWidget(new QLabel("Time"), 2, 0);
	gridLayout->addWidget(timeSpinBox_, 2, 1);

	startButton_ = new QPushButton("Start");
	gridLayout->addWidget(startButton_, 3, 0, 1, 2);
}

void SGMEnergyTrajectoryView::setupData()
{
	startpointSpinBox_->setRange(controlSet_->energyTrajectoryStartpoint()->minimumValue(),
								 controlSet_->energyTrajectoryStartpoint()->maximumValue());
	startpointSpinBox_->setSuffix(controlSet_->energyTrajectoryStartpoint()->units());

	endpointSpinBox_->setRange(controlSet_->energyTrajectoryEndpoint()->minimumValue(),
							   controlSet_->energyTrajectoryEndpoint()->maximumValue());
	endpointSpinBox_->setSuffix(controlSet_->energyTrajectoryEndpoint()->units());

	timeSpinBox_->setRange(controlSet_->energyTrajectoryTime()->minimumValue(),
						   controlSet_->energyTrajectoryTime()->maximumValue());
	timeSpinBox_->setSuffix(controlSet_->energyTrajectoryTime()->units());



}

void SGMEnergyTrajectoryView::setupConnections()
{
	connect(startButton_, SIGNAL(clicked()),
			this, SLOT(onStartPushButtonClicked()));
}



