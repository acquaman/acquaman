#include "SGMHexapodTrajectoryView.h"
#include <QGridLayout>
#include "beamline/SGM/SGMHexapod.h"
#include "beamline/AMControl.h"
SGMHexapodTrajectoryView::SGMHexapodTrajectoryView(SGMHexapod* hexapod,
												   QWidget *parent) :
    QWidget(parent)
{
	hexapod_ = hexapod;
	setupUi();
	if(hexapod_->isConnected()) {
		setupData();
		setupConnections();
	} else {
		connect(hexapod_, SIGNAL(connected(bool)), this, SLOT(onInitialHexapodConnection(bool)));
	}
}


void SGMHexapodTrajectoryView::onInitialHexapodConnection(bool connectedState)
{
	if(connectedState) {
		setupData();
		setupConnections();
	}
}

void SGMHexapodTrajectoryView::onMoveButtonClicked()
{
	hexapod_->trajectoryStart()->move(1);
}

void SGMHexapodTrajectoryView::onResetButtonClicked()
{
	if(hexapod_) {
		hexapod_->trajectoryReset()->move(1);
	}
}

void SGMHexapodTrajectoryView::onXSetpointSpinboxEditingFinished()
{
	if(hexapod_ && !hexapod_->xAxisPrimeTrajectoryControl()->withinTolerance(xSetpointSpinbox_->value())) {
		hexapod_->xAxisPrimeTrajectoryControl()->move(xSetpointSpinbox_->value());
		// Indicate that a value has been set with green bg color
		xSetpointSpinbox_->setStyleSheet("background-color: #d4ffdf;");
	}
}

void SGMHexapodTrajectoryView::onYSetpointSpinboxEditingFinished()
{
	if(hexapod_ && !hexapod_->yAxisPrimeTrajectoryControl()->withinTolerance(ySetpointSpinbox_->value())) {
		hexapod_->yAxisPrimeTrajectoryControl()->move(ySetpointSpinbox_->value());
		// Indicate that a value has been set with green bg color
		ySetpointSpinbox_->setStyleSheet("background-color: #d4ffdf;");
	}
}

void SGMHexapodTrajectoryView::onZSetpointSpinboxEditingFinished()
{
	if(hexapod_ && !hexapod_->zAxisPrimeTrajectoryControl()->withinTolerance(zSetpointSpinbox_->value())) {
		hexapod_->zAxisPrimeTrajectoryControl()->move(zSetpointSpinbox_->value());
		// Indicate that a value has been set with green bg color
		zSetpointSpinbox_->setStyleSheet("background-color: #d4ffdf;");
	}
}

void SGMHexapodTrajectoryView::onXSetpointPVValueChanged()
{
	if(hexapod_ && !hexapod_->xAxisPrimeTrajectoryControl()->withinTolerance(xSetpointSpinbox_->value())) {
		xSetpointSpinbox_->setValue(hexapod_->xAxisPrimeTrajectoryControl()->value());
	}
}

void SGMHexapodTrajectoryView::onYSetpointPVValueChanged()
{
	if(hexapod_ && !hexapod_->yAxisPrimeTrajectoryControl()->withinTolerance(ySetpointSpinbox_->value())) {
		ySetpointSpinbox_->setValue(hexapod_->yAxisPrimeTrajectoryControl()->value());
	}
}

void SGMHexapodTrajectoryView::onZSetpointPVValueChanged()
{
	if(hexapod_ && !hexapod_->zAxisPrimeTrajectoryControl()->withinTolerance(zSetpointSpinbox_->value())) {
		zSetpointSpinbox_->setValue(hexapod_->zAxisPrimeTrajectoryControl()->value());
	}
}

void SGMHexapodTrajectoryView::onStartMovePVChanged(double setpoint)
{
	// Check if value that it was set to was 1 (indicating a move start was pushed)
	if(qAbs(1 - setpoint) > hexapod_->trajectoryStart()->tolerance()) {
		xSetpointSpinbox_->setStyleSheet("background-color: #ffffff");
		ySetpointSpinbox_->setStyleSheet("background-color: #ffffff");
		zSetpointSpinbox_->setStyleSheet("background-color: #ffffff");
	}
}

void SGMHexapodTrajectoryView::setupUi()
{
	xSetpointLabel_ = new QLabel();
	ySetpointLabel_ = new QLabel();
	zSetpointLabel_ = new QLabel();

	xSetpointSpinbox_ = new QDoubleSpinBox();
	ySetpointSpinbox_ = new QDoubleSpinBox();
	zSetpointSpinbox_ = new QDoubleSpinBox();

	moveButton_ = new QPushButton("Move");
	resetButton_ = new QPushButton("Reset");

	QGridLayout* mainLayout = new QGridLayout();
	setLayout(mainLayout);

	mainLayout->addWidget(xSetpointLabel_, 1, 1, Qt::AlignRight);
	mainLayout->addWidget(ySetpointLabel_, 3, 1, Qt::AlignRight);
	mainLayout->addWidget(zSetpointLabel_, 5, 1, Qt::AlignRight);

	mainLayout->addWidget(xSetpointSpinbox_, 1, 2, Qt::AlignHCenter);
	mainLayout->addWidget(ySetpointSpinbox_, 3, 2, Qt::AlignHCenter);
	mainLayout->addWidget(zSetpointSpinbox_, 5, 2, Qt::AlignHCenter);

	mainLayout->addWidget(moveButton_, 7, 1, 1, 2, Qt::AlignHCenter);
	mainLayout->addWidget(resetButton_,7, 4, 1, 2, Qt::AlignHCenter);

}

void SGMHexapodTrajectoryView::setupData()
{
	if(hexapod_) {

		xSetpointLabel_->setText(hexapod_->xAxisPrimeControl()->name());
		ySetpointLabel_->setText(hexapod_->yAxisPrimeControl()->name());
		zSetpointLabel_->setText(hexapod_->zAxisPrimeControl()->name());

		xSetpointSpinbox_->setValue(hexapod_->xAxisPrimeControl()->value());
		ySetpointSpinbox_->setValue(hexapod_->yAxisPrimeControl()->value());
		zSetpointSpinbox_->setValue(hexapod_->zAxisPrimeControl()->value());

		xSetpointSpinbox_->setRange(hexapod_->xAxisPrimeControl()->minimumValue(), hexapod_->xAxisPrimeControl()->maximumValue());
		ySetpointSpinbox_->setRange(hexapod_->yAxisPrimeControl()->minimumValue(), hexapod_->yAxisPrimeControl()->maximumValue());
		zSetpointSpinbox_->setRange(hexapod_->zAxisPrimeControl()->minimumValue(), hexapod_->zAxisPrimeControl()->maximumValue());
	}
}

void SGMHexapodTrajectoryView::setupConnections()
{
	connect(moveButton_, SIGNAL(clicked()), this, SLOT(onMoveButtonClicked()));
	connect(resetButton_, SIGNAL(clicked()), this, SLOT(onResetButtonClicked()));

	connect(xSetpointSpinbox_, SIGNAL(editingFinished()), this, SLOT(onXSetpointSpinboxEditingFinished()));
	connect(ySetpointSpinbox_, SIGNAL(editingFinished()), this, SLOT(onYSetpointSpinboxEditingFinished()));
	connect(zSetpointSpinbox_, SIGNAL(editingFinished()), this, SLOT(onZSetpointSpinboxEditingFinished()));

	if(hexapod_) {
		connect(hexapod_->xAxisPrimeTrajectoryControl(), SIGNAL(valueChanged(double)), this, SLOT(onXSetpointPVValueChanged()));
		connect(hexapod_->yAxisPrimeTrajectoryControl(), SIGNAL(valueChanged(double)), this, SLOT(onYSetpointPVValueChanged()));
		connect(hexapod_->zAxisPrimeTrajectoryControl(), SIGNAL(valueChanged(double)), this, SLOT(onZSetpointPVValueChanged()));

		connect(hexapod_->trajectoryStart(), SIGNAL(setpointChanged(double)), this, SLOT(onStartMovePVChanged(double)));
	}
}
