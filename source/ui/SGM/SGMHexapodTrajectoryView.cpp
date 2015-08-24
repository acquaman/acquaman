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
	// Reset colours on move:
	xSetpointSpinbox_->setStyleSheet("background-color: #ffffff;");
	ySetpointSpinbox_->setStyleSheet("background-color: #ffffff;");
	zSetpointSpinbox_->setStyleSheet("background-color: #ffffff;");
	uSetpointSpinbox_->setStyleSheet("background-color: #ffffff;");
	vSetpointSpinbox_->setStyleSheet("background-color: #ffffff;");
	wSetpointSpinbox_->setStyleSheet("background-color: #ffffff;");
}

void SGMHexapodTrajectoryView::onResetButtonClicked()
{
	if(hexapod_) {
		hexapod_->trajectoryReset()->move(1);
	}
}

void SGMHexapodTrajectoryView::onXSetpointSpinboxEditingFinished()
{
	if(hexapod_ && !hexapod_->xAxisTrajectorySetpoint()->withinTolerance(xSetpointSpinbox_->value())) {
		hexapod_->xAxisTrajectorySetpoint()->move(xSetpointSpinbox_->value());
		// Indicate that a value has been set with green bg color
		xSetpointSpinbox_->setStyleSheet("background-color: #d4ffdf;");
	}
}

void SGMHexapodTrajectoryView::onYSetpointSpinboxEditingFinished()
{
	if(hexapod_ && !hexapod_->yAxisTrajectorySetpoint()->withinTolerance(ySetpointSpinbox_->value())) {
		hexapod_->yAxisTrajectorySetpoint()->move(ySetpointSpinbox_->value());
		// Indicate that a value has been set with green bg color
		ySetpointSpinbox_->setStyleSheet("background-color: #d4ffdf;");
	}
}

void SGMHexapodTrajectoryView::onZSetpointSpinboxEditingFinished()
{
	if(hexapod_ && !hexapod_->zAxisTrajectorySetpoint()->withinTolerance(zSetpointSpinbox_->value())) {
		hexapod_->zAxisTrajectorySetpoint()->move(zSetpointSpinbox_->value());
		// Indicate that a value has been set with green bg color
		zSetpointSpinbox_->setStyleSheet("background-color: #d4ffdf;");
	}
}

void SGMHexapodTrajectoryView::onUSetpointSpinboxEditingFinished()
{
	if(hexapod_ && !hexapod_->uAxisTrajectorySetpoint()->withinTolerance(uSetpointSpinbox_->value())) {
		hexapod_->uAxisTrajectorySetpoint()->move(uSetpointSpinbox_->value());
		// Indicate that a value has been set with green bg color
		uSetpointSpinbox_->setStyleSheet("background-color: #d4ffdf;");
	}
}

void SGMHexapodTrajectoryView::onVSetpointSpinboxEditingFinished()
{
	if(hexapod_ && !hexapod_->vAxisTrajectorySetpoint()->withinTolerance(vSetpointSpinbox_->value())) {
		hexapod_->vAxisTrajectorySetpoint()->move(vSetpointSpinbox_->value());
		// Indicate that a value has been set with green bg color
		vSetpointSpinbox_->setStyleSheet("background-color: #d4ffdf;");
	}
}

void SGMHexapodTrajectoryView::onWSetpointSpinboxEditingFinished()
{
	if(hexapod_ && !hexapod_->wAxisTrajectorySetpoint()->withinTolerance(wSetpointSpinbox_->value())) {
		hexapod_->wAxisTrajectorySetpoint()->move(wSetpointSpinbox_->value());
		// Indicate that a value has been set with green bg color
		wSetpointSpinbox_->setStyleSheet("background-color: #d4ffdf;");
	}
}

void SGMHexapodTrajectoryView::onXSetpointPVValueChanged()
{
	if(hexapod_ && !hexapod_->xAxisTrajectorySetpoint()->withinTolerance(xSetpointSpinbox_->value())) {
		xSetpointSpinbox_->setValue(hexapod_->xAxisTrajectorySetpoint()->value());
		// Indicate a set back to PV value with white bg color
		xSetpointSpinbox_->setStyleSheet("background-color: #ffffff;");
	}
}
#include <QDebug>
void SGMHexapodTrajectoryView::onYSetpointPVValueChanged()
{
	if(hexapod_ && !hexapod_->yAxisTrajectorySetpoint()->withinTolerance(ySetpointSpinbox_->value())) {
		qDebug() << "\tUpdating Y Spinbox value to " << hexapod_->yAxisTrajectorySetpoint()->value();
		ySetpointSpinbox_->setValue(hexapod_->yAxisTrajectorySetpoint()->value());
		// Indicate a set back to PV value with white bg color
		ySetpointSpinbox_->setStyleSheet("background-color: #ffffff;");
	}
}

void SGMHexapodTrajectoryView::onZSetpointPVValueChanged()
{
	if(hexapod_ && !hexapod_->zAxisTrajectorySetpoint()->withinTolerance(zSetpointSpinbox_->value())) {
		zSetpointSpinbox_->setValue(hexapod_->zAxisTrajectorySetpoint()->value());
		// Indicate a set back to PV value with white bg color
		zSetpointSpinbox_->setStyleSheet("background-color: #ffffff;");
	}
}

void SGMHexapodTrajectoryView::onUSetpointPVValueChanged()
{
	if(hexapod_ && !hexapod_->uAxisTrajectorySetpoint()->withinTolerance(uSetpointSpinbox_->value())) {
		uSetpointSpinbox_->setValue(hexapod_->uAxisTrajectorySetpoint()->value());
		// Indicate a set back to PV value with white bg color
		uSetpointSpinbox_->setStyleSheet("background-color: #ffffff;");
	}
}

void SGMHexapodTrajectoryView::onVSetpointPVValueChanged()
{
	if(hexapod_ && !hexapod_->vAxisTrajectorySetpoint()->withinTolerance(vSetpointSpinbox_->value())) {
		vSetpointSpinbox_->setValue(hexapod_->vAxisTrajectorySetpoint()->value());
		// Indicate a set back to PV value with white bg color
		vSetpointSpinbox_->setStyleSheet("background-color: #ffffff;");
	}
}

void SGMHexapodTrajectoryView::onWSetpointPVValueChanged()
{
	if(hexapod_ && !hexapod_->wAxisTrajectorySetpoint()->withinTolerance(wSetpointSpinbox_->value())) {
		wSetpointSpinbox_->setValue(hexapod_->wAxisTrajectorySetpoint()->value());
		// Indicate a set back to PV value with white bg color
		wSetpointSpinbox_->setStyleSheet("background-color: #ffffff;");
	}
}

void SGMHexapodTrajectoryView::setupUi()
{
	xSetpointLabel_ = new QLabel();
	ySetpointLabel_ = new QLabel();
	zSetpointLabel_ = new QLabel();
	uSetpointLabel_ = new QLabel();
	vSetpointLabel_ = new QLabel();
	wSetpointLabel_ = new QLabel();

	xSetpointSpinbox_ = new QDoubleSpinBox();
	ySetpointSpinbox_ = new QDoubleSpinBox();
	zSetpointSpinbox_ = new QDoubleSpinBox();
	uSetpointSpinbox_ = new QDoubleSpinBox();
	vSetpointSpinbox_ = new QDoubleSpinBox();
	wSetpointSpinbox_ = new QDoubleSpinBox();

	moveButton_ = new QPushButton("Move");
	resetButton_ = new QPushButton("Reset");

	QGridLayout* mainLayout = new QGridLayout();
	setLayout(mainLayout);

	mainLayout->addWidget(xSetpointLabel_, 1, 1, Qt::AlignRight);
	mainLayout->addWidget(ySetpointLabel_, 3, 1, Qt::AlignRight);
	mainLayout->addWidget(zSetpointLabel_, 5, 1, Qt::AlignRight);
	mainLayout->addWidget(uSetpointLabel_, 1, 4, Qt::AlignRight);
	mainLayout->addWidget(vSetpointLabel_, 3, 4, Qt::AlignRight);
	mainLayout->addWidget(wSetpointLabel_, 5, 4, Qt::AlignRight);

	mainLayout->addWidget(xSetpointSpinbox_, 1, 2, Qt::AlignHCenter);
	mainLayout->addWidget(ySetpointSpinbox_, 3, 2, Qt::AlignHCenter);
	mainLayout->addWidget(zSetpointSpinbox_, 5, 2, Qt::AlignHCenter);
	mainLayout->addWidget(uSetpointSpinbox_, 1, 5, Qt::AlignHCenter);
	mainLayout->addWidget(vSetpointSpinbox_, 3, 5, Qt::AlignHCenter);
	mainLayout->addWidget(wSetpointSpinbox_, 5, 5, Qt::AlignHCenter);

	mainLayout->addWidget(moveButton_, 7, 1, 1, 2, Qt::AlignHCenter);
	mainLayout->addWidget(resetButton_,7, 4, 1, 2, Qt::AlignHCenter);
}
#include <QDebug>
void SGMHexapodTrajectoryView::setupData()
{
	if(hexapod_) {
		qDebug() << "\tInitializing Y Axis value to " << hexapod_->yAxis()->value();
		xSetpointLabel_->setText(hexapod_->xAxis()->name());
		ySetpointLabel_->setText(hexapod_->yAxis()->name());
		zSetpointLabel_->setText(hexapod_->zAxis()->name());
		uSetpointLabel_->setText(hexapod_->uAxis()->name());
		vSetpointLabel_->setText(hexapod_->vAxis()->name());
		wSetpointLabel_->setText(hexapod_->wAxis()->name());

		xSetpointSpinbox_->setValue(hexapod_->xAxis()->value());
		ySetpointSpinbox_->setValue(hexapod_->yAxis()->value());
		zSetpointSpinbox_->setValue(hexapod_->zAxis()->value());
		uSetpointSpinbox_->setValue(hexapod_->uAxis()->value());
		vSetpointSpinbox_->setValue(hexapod_->vAxis()->value());
		wSetpointSpinbox_->setValue(hexapod_->wAxis()->value());

		xSetpointSpinbox_->setRange(hexapod_->xAxis()->minimumValue(), hexapod_->xAxis()->maximumValue());
		ySetpointSpinbox_->setRange(hexapod_->yAxis()->minimumValue(), hexapod_->yAxis()->maximumValue());
		zSetpointSpinbox_->setRange(hexapod_->zAxis()->minimumValue(), hexapod_->zAxis()->maximumValue());
		uSetpointSpinbox_->setRange(hexapod_->uAxis()->minimumValue(), hexapod_->uAxis()->maximumValue());
		vSetpointSpinbox_->setRange(hexapod_->vAxis()->minimumValue(), hexapod_->vAxis()->maximumValue());
		wSetpointSpinbox_->setRange(hexapod_->wAxis()->minimumValue(), hexapod_->wAxis()->maximumValue());

	}
}

void SGMHexapodTrajectoryView::setupConnections()
{
	connect(moveButton_, SIGNAL(clicked()), this, SLOT(onMoveButtonClicked()));
	connect(resetButton_, SIGNAL(clicked()), this, SLOT(onResetButtonClicked()));

	connect(xSetpointSpinbox_, SIGNAL(editingFinished()), this, SLOT(onXSetpointSpinboxEditingFinished()));
	connect(ySetpointSpinbox_, SIGNAL(editingFinished()), this, SLOT(onYSetpointSpinboxEditingFinished()));
	connect(zSetpointSpinbox_, SIGNAL(editingFinished()), this, SLOT(onZSetpointSpinboxEditingFinished()));
	connect(uSetpointSpinbox_, SIGNAL(editingFinished()), this, SLOT(onUSetpointSpinboxEditingFinished()));
	connect(vSetpointSpinbox_, SIGNAL(editingFinished()), this, SLOT(onVSetpointSpinboxEditingFinished()));
	connect(wSetpointSpinbox_, SIGNAL(editingFinished()), this, SLOT(onWSetpointSpinboxEditingFinished()));

	if(hexapod_) {
		connect(hexapod_->xAxisTrajectorySetpoint(), SIGNAL(valueChanged(double)), this, SLOT(onXSetpointPVValueChanged()));
		connect(hexapod_->yAxisTrajectorySetpoint(), SIGNAL(valueChanged(double)), this, SLOT(onYSetpointPVValueChanged()));
		connect(hexapod_->zAxisTrajectorySetpoint(), SIGNAL(valueChanged(double)), this, SLOT(onZSetpointPVValueChanged()));
		connect(hexapod_->uAxisTrajectorySetpoint(), SIGNAL(valueChanged(double)), this, SLOT(onUSetpointPVValueChanged()));
		connect(hexapod_->vAxisTrajectorySetpoint(), SIGNAL(valueChanged(double)), this, SLOT(onVSetpointPVValueChanged()));
		connect(hexapod_->wAxisTrajectorySetpoint(), SIGNAL(valueChanged(double)), this, SLOT(onWSetpointPVValueChanged()));
	}
}
