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
//	uSetpointSpinbox_->setStyleSheet("background-color: #ffffff;");
//	vSetpointSpinbox_->setStyleSheet("background-color: #ffffff;");
//	wSetpointSpinbox_->setStyleSheet("background-color: #ffffff;");
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

void SGMHexapodTrajectoryView::onUSetpointSpinboxEditingFinished()
{
//	if(hexapod_ && !hexapod_->uAxisPrimeTrajectoryControl()->withinTolerance(uSetpointSpinbox_->value())) {
//		hexapod_->uAxisPrimeTrajectoryControl()->move(uSetpointSpinbox_->value());
//		// Indicate that a value has been set with green bg color
//		uSetpointSpinbox_->setStyleSheet("background-color: #d4ffdf;");
//	}
}

void SGMHexapodTrajectoryView::onVSetpointSpinboxEditingFinished()
{
//	if(hexapod_ && !hexapod_->vAxisPrimeTrajectoryControl()->withinTolerance(vSetpointSpinbox_->value())) {
//		hexapod_->vAxisPrimeTrajectoryControl()->move(vSetpointSpinbox_->value());
//		// Indicate that a value has been set with green bg color
//		vSetpointSpinbox_->setStyleSheet("background-color: #d4ffdf;");
//	}
}

void SGMHexapodTrajectoryView::onWSetpointSpinboxEditingFinished()
{
//	if(hexapod_ && !hexapod_->wAxisPrimeTrajectoryControl()->withinTolerance(wSetpointSpinbox_->value())) {
//		hexapod_->wAxisPrimeTrajectoryControl()->move(wSetpointSpinbox_->value());
//		// Indicate that a value has been set with green bg color
//		wSetpointSpinbox_->setStyleSheet("background-color: #d4ffdf;");
//	}
}

void SGMHexapodTrajectoryView::onXSetpointPVValueChanged()
{
	if(hexapod_ && !hexapod_->xAxisPrimeTrajectoryControl()->withinTolerance(xSetpointSpinbox_->value())) {
		xSetpointSpinbox_->setValue(hexapod_->xAxisPrimeTrajectoryControl()->value());
		// Indicate a set back to PV value with white bg color
		xSetpointSpinbox_->setStyleSheet("background-color: #ffffff;");
	}
}

void SGMHexapodTrajectoryView::onYSetpointPVValueChanged()
{
	if(hexapod_ && !hexapod_->yAxisPrimeTrajectoryControl()->withinTolerance(ySetpointSpinbox_->value())) {
		ySetpointSpinbox_->setValue(hexapod_->yAxisPrimeTrajectoryControl()->value());
		// Indicate a set back to PV value with white bg color
		ySetpointSpinbox_->setStyleSheet("background-color: #ffffff;");
	}
}

void SGMHexapodTrajectoryView::onZSetpointPVValueChanged()
{
	if(hexapod_ && !hexapod_->zAxisPrimeTrajectoryControl()->withinTolerance(zSetpointSpinbox_->value())) {
		zSetpointSpinbox_->setValue(hexapod_->zAxisPrimeTrajectoryControl()->value());
		// Indicate a set back to PV value with white bg color
		zSetpointSpinbox_->setStyleSheet("background-color: #ffffff;");
	}
}

void SGMHexapodTrajectoryView::onUSetpointPVValueChanged()
{
//	if(hexapod_ && !hexapod_->uAxisPrimeTrajectoryControl()->withinTolerance(uSetpointSpinbox_->value())) {
//		uSetpointSpinbox_->setValue(hexapod_->uAxisPrimeTrajectoryControl()->value());
//		// Indicate a set back to PV value with white bg color
//		uSetpointSpinbox_->setStyleSheet("background-color: #ffffff;");
//	}
}

void SGMHexapodTrajectoryView::onVSetpointPVValueChanged()
{
//	if(hexapod_ && !hexapod_->vAxisPrimeTrajectoryControl()->withinTolerance(vSetpointSpinbox_->value())) {
//		vSetpointSpinbox_->setValue(hexapod_->vAxisPrimeTrajectoryControl()->value());
//		// Indicate a set back to PV value with white bg color
//		vSetpointSpinbox_->setStyleSheet("background-color: #ffffff;");
//	}
}

void SGMHexapodTrajectoryView::onWSetpointPVValueChanged()
{
//	if(hexapod_ && !hexapod_->wAxisPrimeTrajectoryControl()->withinTolerance(wSetpointSpinbox_->value())) {
//		wSetpointSpinbox_->setValue(hexapod_->wAxisPrimeTrajectoryControl()->value());
//		// Indicate a set back to PV value with white bg color
//		wSetpointSpinbox_->setStyleSheet("background-color: #ffffff;");
//	}
}

void SGMHexapodTrajectoryView::setupUi()
{
	xSetpointLabel_ = new QLabel();
	ySetpointLabel_ = new QLabel();
	zSetpointLabel_ = new QLabel();
//	uSetpointLabel_ = new QLabel();
//	vSetpointLabel_ = new QLabel();
//	wSetpointLabel_ = new QLabel();

	xSetpointSpinbox_ = new QDoubleSpinBox();
	ySetpointSpinbox_ = new QDoubleSpinBox();
	zSetpointSpinbox_ = new QDoubleSpinBox();
//	uSetpointSpinbox_ = new QDoubleSpinBox();
//	vSetpointSpinbox_ = new QDoubleSpinBox();
//	wSetpointSpinbox_ = new QDoubleSpinBox();

	moveButton_ = new QPushButton("Move");
	resetButton_ = new QPushButton("Reset");

	QGridLayout* mainLayout = new QGridLayout();
	setLayout(mainLayout);

	mainLayout->addWidget(xSetpointLabel_, 1, 1, Qt::AlignRight);
	mainLayout->addWidget(ySetpointLabel_, 3, 1, Qt::AlignRight);
	mainLayout->addWidget(zSetpointLabel_, 5, 1, Qt::AlignRight);
//	mainLayout->addWidget(uSetpointLabel_, 1, 4, Qt::AlignRight);
//	mainLayout->addWidget(vSetpointLabel_, 3, 4, Qt::AlignRight);
//	mainLayout->addWidget(wSetpointLabel_, 5, 4, Qt::AlignRight);

	mainLayout->addWidget(xSetpointSpinbox_, 1, 2, Qt::AlignHCenter);
	mainLayout->addWidget(ySetpointSpinbox_, 3, 2, Qt::AlignHCenter);
	mainLayout->addWidget(zSetpointSpinbox_, 5, 2, Qt::AlignHCenter);
//	mainLayout->addWidget(uSetpointSpinbox_, 1, 5, Qt::AlignHCenter);
//	mainLayout->addWidget(vSetpointSpinbox_, 3, 5, Qt::AlignHCenter);
//	mainLayout->addWidget(wSetpointSpinbox_, 5, 5, Qt::AlignHCenter);

	mainLayout->addWidget(moveButton_, 7, 1, 1, 2, Qt::AlignHCenter);
	mainLayout->addWidget(resetButton_,7, 4, 1, 2, Qt::AlignHCenter);

	mainLayout->addWidget(statuses_, 1,5);
}

void SGMHexapodTrajectoryView::setupData()
{
	if(hexapod_) {

		xSetpointLabel_->setText(hexapod_->xAxisPrimeControl()->name());
		ySetpointLabel_->setText(hexapod_->yAxisPrimeControl()->name());
		zSetpointLabel_->setText(hexapod_->zAxisPrimeControl()->name());
//		uSetpointLabel_->setText(hexapod_->uAxis()->name());
//		vSetpointLabel_->setText(hexapod_->vAxis()->name());
//		wSetpointLabel_->setText(hexapod_->wAxis()->name());

		xSetpointSpinbox_->setValue(hexapod_->xAxisPrimeControl()->value());
		ySetpointSpinbox_->setValue(hexapod_->yAxisPrimeControl()->value());
		zSetpointSpinbox_->setValue(hexapod_->zAxisPrimeControl()->value());
//		uSetpointSpinbox_->setValue(hexapod_->uAxis()->value());
//		vSetpointSpinbox_->setValue(hexapod_->vAxis()->value());
//		wSetpointSpinbox_->setValue(hexapod_->wAxis()->value());

		xSetpointSpinbox_->setRange(hexapod_->xAxisPrimeControl()->minimumValue(), hexapod_->xAxisPrimeControl()->maximumValue());
		ySetpointSpinbox_->setRange(hexapod_->yAxisPrimeControl()->minimumValue(), hexapod_->yAxisPrimeControl()->maximumValue());
		zSetpointSpinbox_->setRange(hexapod_->zAxisPrimeControl()->minimumValue(), hexapod_->zAxisPrimeControl()->maximumValue());
//		uSetpointSpinbox_->setRange(hexapod_->uAxis()->minimumValue(), hexapod_->uAxis()->maximumValue());
//		vSetpointSpinbox_->setRange(hexapod_->vAxis()->minimumValue(), hexapod_->vAxis()->maximumValue());
//		wSetpointSpinbox_->setRange(hexapod_->wAxis()->minimumValue(), hexapod_->wAxis()->maximumValue());

	}
}

void SGMHexapodTrajectoryView::setupConnections()
{
	connect(moveButton_, SIGNAL(clicked()), this, SLOT(onMoveButtonClicked()));
	connect(resetButton_, SIGNAL(clicked()), this, SLOT(onResetButtonClicked()));

	connect(xSetpointSpinbox_, SIGNAL(editingFinished()), this, SLOT(onXSetpointSpinboxEditingFinished()));
	connect(ySetpointSpinbox_, SIGNAL(editingFinished()), this, SLOT(onYSetpointSpinboxEditingFinished()));
	connect(zSetpointSpinbox_, SIGNAL(editingFinished()), this, SLOT(onZSetpointSpinboxEditingFinished()));
//	connect(uSetpointSpinbox_, SIGNAL(editingFinished()), this, SLOT(onUSetpointSpinboxEditingFinished()));
//	connect(vSetpointSpinbox_, SIGNAL(editingFinished()), this, SLOT(onVSetpointSpinboxEditingFinished()));
//	connect(wSetpointSpinbox_, SIGNAL(editingFinished()), this, SLOT(onWSetpointSpinboxEditingFinished()));

	if(hexapod_) {
		connect(hexapod_->xAxisPrimeTrajectoryControl(), SIGNAL(valueChanged(double)), this, SLOT(onXSetpointPVValueChanged()));
		connect(hexapod_->yAxisPrimeTrajectoryControl(), SIGNAL(valueChanged(double)), this, SLOT(onYSetpointPVValueChanged()));
		connect(hexapod_->zAxisPrimeTrajectoryControl(), SIGNAL(valueChanged(double)), this, SLOT(onZSetpointPVValueChanged()));
//		connect(hexapod_->uAxisPrimeTrajectoryControl(), SIGNAL(valueChanged(double)), this, SLOT(onUSetpointPVValueChanged()));
//		connect(hexapod_->vAxisPrimeTrajectoryControl(), SIGNAL(valueChanged(double)), this, SLOT(onVSetpointPVValueChanged()));
//		connect(hexapod_->wAxisPrimeTrajectoryControl(), SIGNAL(valueChanged(double)), this, SLOT(onWSetpointPVValueChanged()));
	}
}
