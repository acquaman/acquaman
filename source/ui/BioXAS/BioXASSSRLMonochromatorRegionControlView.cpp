#include "BioXASSSRLMonochromatorRegionControlView.h"
#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorRegionControl.h"

BioXASSSRLMonochromatorRegionControlView::BioXASSSRLMonochromatorRegionControlView(BioXASSSRLMonochromatorRegionControl *regionControl, QWidget *parent) :
	QWidget(parent)
{
	// Initialize member variables.

	regionControl_ = 0;

	// Create UI elements.

	QLabel *slitsStatusPrompt = new QLabel("Slits status:");
	slitsStatusGreen_ = new QLabel();
	slitsStatusRed_ = new QLabel();

	QLabel *paddleStatusPrompt = new QLabel("Paddle status:");
	paddleStatusGreen_ = new QLabel();
	paddleStatusRed_ = new QLabel();

	QLabel *keyStatusPrompt = new QLabel("Key status:");
	keyStatusGreen_ = new QLabel();
	keyStatusRed_ = new QLabel();

	QLabel *brakeStatusPrompt = new QLabel("Brake status:");
	brakeStatusGreen_ = new QLabel();
	brakeStatusRed_ = new QLabel();

	QLabel *braggInPositionPrompt = new QLabel("Bragg position status:");
	braggInPositionGreen_ = new QLabel();
	braggInPositionRed_ = new QLabel();

	QLabel *regionAStatusPrompt = new QLabel("Region A status:");
	regionAStatusGreen_ = new QLabel();
	regionAStatusRed_ = new QLabel();

	QLabel *regionBStatusPrompt = new QLabel("Region B status:");
	regionBStatusGreen_ = new QLabel();
	regionBStatusRed_ = new QLabel();

	// Create and set layouts.

	QGridLayout *statusLayout = new QGridLayout();
	statusLayout->addWidget(slitsStatusPrompt, 0, 0, 1, 1, Qt::AlignRight);
	statusLayout->addWidget(slitsStatusGreen_, 0, 1, 1, 1, Qt::AlignCenter);
	statusLayout->addWidget(slitsStatusRed_, 0, 2, 1, 1, Qt::AlignCenter);

	statusLayout->addWidget(paddleStatusPrompt, 1, 0, 1, 1, Qt::AlignRight);
	statusLayout->addWidget(paddleStatusGreen_, 1, 1, 1, 1, Qt::AlignCenter);
	statusLayout->addWidget(paddleStatusRed_, 1, 2, 1, 1, Qt::AlignCenter);

	statusLayout->addWidget(keyStatusPrompt, 2, 0, 1, 1, Qt::AlignRight);
	statusLayout->addWidget(keyStatusGreen_, 2, 1, 1, 1, Qt::AlignCenter);
	statusLayout->addWidget(keyStatusRed_, 2, 2, 1, 1, Qt::AlignCenter);

	statusLayout->addWidget(brakeStatusPrompt, 3, 0, 1, 1, Qt::AlignRight);
	statusLayout->addWidget(brakeStatusGreen_, 3, 1, 1, 1, Qt::AlignCenter);
	statusLayout->addWidget(brakeStatusRed_, 3, 2, 1, 1, Qt::AlignCenter);

	statusLayout->addWidget(braggInPositionPrompt, 4, 0, 1, 1, Qt::AlignRight);
	statusLayout->addWidget(braggInPositionGreen_, 4, 1, 1, 1, Qt::AlignCenter);
	statusLayout->addWidget(braggInPositionRed_, 4, 2, 1, 1, Qt::AlignCenter);

	statusLayout->addWidget(regionAStatusPrompt, 5, 0, 1, 1, Qt::AlignRight);
	statusLayout->addWidget(regionAStatusGreen_, 5, 1, 1, 1, Qt::AlignCenter);
	statusLayout->addWidget(regionAStatusRed_, 5, 2, 1, 1, Qt::AlignCenter);

	statusLayout->addWidget(regionBStatusPrompt, 6, 0, 1, 1, Qt::AlignRight);
	statusLayout->addWidget(regionBStatusGreen_, 6, 1, 1, 1, Qt::AlignCenter);
	statusLayout->addWidget(regionBStatusRed_, 6, 2, 1, 1, Qt::AlignCenter);

	setLayout(statusLayout);

	// Initial settings.

	slitsStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
	slitsStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

	paddleStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
	paddleStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

	keyStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
	keyStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

	brakeStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
	brakeStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

	braggInPositionGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
	braggInPositionRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

	regionAStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
	regionAStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

	regionBStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
	regionBStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

	// Current settings.

	setRegionControl(regionControl);
}

BioXASSSRLMonochromatorRegionControlView::~BioXASSSRLMonochromatorRegionControlView()
{

}

void BioXASSSRLMonochromatorRegionControlView::setRegionControl(BioXASSSRLMonochromatorRegionControl *newControl)
{
	if (regionControl_ != newControl) {

		if (regionControl_)
			disconnect( regionControl_, 0, this, 0 );

		regionControl_ = newControl;

		if (regionControl_) {
			connect( regionControl_->slitsStatusControl(), SIGNAL(connected(bool)), this, SLOT(onSlitsStatusChanged()) );
			connect( regionControl_->slitsStatusControl(), SIGNAL(valueChanged(double)), this, SLOT(onSlitsStatusChanged()) );

			connect( regionControl_->paddleStatusControl(), SIGNAL(connected(bool)), this, SLOT(onPaddleStatusChanged()) );
			connect( regionControl_->paddleStatusControl(), SIGNAL(valueChanged(double)), this, SLOT(onPaddleStatusChanged()) );

			connect( regionControl_->keyStatusControl(), SIGNAL(connected(bool)), this, SLOT(onKeyStatusChanged()) );
			connect( regionControl_->keyStatusControl(), SIGNAL(valueChanged(double)), this, SLOT(onKeyStatusChanged()) );

			connect( regionControl_->brakeStatusControl(), SIGNAL(connected(bool)), this, SLOT(onBrakeStatusChanged()) );
			connect( regionControl_->brakeStatusControl(), SIGNAL(valueChanged(double)), this, SLOT(onBrakeStatusChanged()) );

			connect( regionControl_->braggAtCrystalChangePositionStatusControl(), SIGNAL(connected(bool)), this, SLOT(onBraggAtCrystalChangePositionStatusChanged()) );
			connect( regionControl_->braggAtCrystalChangePositionStatusControl(), SIGNAL(valueChanged(double)), this, SLOT(onBraggAtCrystalChangePositionStatusChanged()) );

			connect( regionControl_->regionAStatusControl(), SIGNAL(connected(bool)), this, SLOT(onRegionAStatusChanged()) );
			connect( regionControl_->regionAStatusControl(), SIGNAL(valueChanged(double)), this, SLOT(onRegionAStatusChanged()) );

			connect( regionControl_->regionBStatusControl(), SIGNAL(connected(bool)), this, SLOT(onRegionBStatusChanged()) );
			connect( regionControl_->regionBStatusControl(), SIGNAL(valueChanged(double)), this, SLOT(onRegionBStatusChanged()) );
		}

		onSlitsStatusChanged();
		onPaddleStatusChanged();
		onKeyStatusChanged();
		onBrakeStatusChanged();
		onBraggAtCrystalChangePositionStatusChanged();
		onRegionAStatusChanged();
		onRegionBStatusChanged();
	}
}

void BioXASSSRLMonochromatorRegionControlView::onSlitsStatusChanged()
{
	if (regionControl_ && regionControl_->slitsStatusControl() && regionControl_->slitsStatusControl()->isConnected()) {
		if (regionControl_->slitsStatusControl()->value() == BioXASSSRLMonochromator::Slits::Closed) {
			slitsStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
			slitsStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

		} else if (regionControl_->slitsStatusControl()->value() == BioXASSSRLMonochromator::Slits::NotClosed){
			slitsStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
			slitsStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOn.png"));

		} else {
			slitsStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
			slitsStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
		}

	} else {
		slitsStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
		slitsStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
	}
}

void BioXASSSRLMonochromatorRegionControlView::onPaddleStatusChanged()
{
	if (regionControl_ && regionControl_->paddleStatusControl() && regionControl_->paddleStatusControl()->isConnected()) {
		if (regionControl_->paddleStatusControl()->value() == BioXASSSRLMonochromator::Paddle::Out) {
			paddleStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
			paddleStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

		} else if (regionControl_->paddleStatusControl()->value() == BioXASSSRLMonochromator::Paddle::NotOut){
			paddleStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
			paddleStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOn.png"));

		}  else {
			paddleStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
			paddleStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
		}

	} else {
		paddleStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
		paddleStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
	}
}

void BioXASSSRLMonochromatorRegionControlView::onKeyStatusChanged()
{
	if (regionControl_ && regionControl_->keyStatusControl() && regionControl_->keyStatusControl()->isConnected()) {
		if (regionControl_->keyStatusControl()->value() == BioXASSSRLMonochromator::Key::Enabled) {
			keyStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
			keyStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

		} else if (regionControl_->keyStatusControl()->value() == BioXASSSRLMonochromator::Key::Disabled){
			keyStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
			keyStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOn.png"));

		} else {
			keyStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
			keyStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
		}

	} else {
		keyStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
		keyStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
	}
}

void BioXASSSRLMonochromatorRegionControlView::onBrakeStatusChanged()
{
	if (regionControl_ && regionControl_->brakeStatusControl() && regionControl_->brakeStatusControl()->isConnected()) {
		if (regionControl_->brakeStatusControl()->value() == BioXASSSRLMonochromator::Brake::Enabled) {
			brakeStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
			brakeStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

		} else if (regionControl_->brakeStatusControl()->value() == BioXASSSRLMonochromator::Brake::Disabled){
			brakeStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
			brakeStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOn.png"));

		} else {
			brakeStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
			brakeStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
		}

	} else {
		brakeStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
		brakeStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
	}
}

#include <QDebug>
void BioXASSSRLMonochromatorRegionControlView::onBraggAtCrystalChangePositionStatusChanged()
{
	if (regionControl_ && regionControl_->braggAtCrystalChangePositionStatusControl() && regionControl_->braggAtCrystalChangePositionStatusControl()->isConnected()) {
		if (regionControl_->braggAtCrystalChangePositionStatusControl()->value() == BioXASSSRLMonochromator::Bragg::InPosition) {
			brakeStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
			brakeStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

		} else if (regionControl_->braggAtCrystalChangePositionStatusControl()->value() == BioXASSSRLMonochromator::Bragg::NotInPosition){
			brakeStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
			brakeStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOn.png"));

		} else {
			qDebug() << "Bragg status in ambiguous state.";
			brakeStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
			brakeStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
		}

	} else {
		qDebug() << "Bragg status not connected.";
		brakeStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
		brakeStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
	}
}

void BioXASSSRLMonochromatorRegionControlView::onRegionAStatusChanged()
{
	if (regionControl_ && regionControl_->regionAStatusControl() && regionControl_->regionAStatusControl()->isConnected()) {
		if (regionControl_->regionAStatusControl()->value() == BioXASSSRLMonochromator::Region::In) {
			regionAStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
			regionAStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

		} else if (regionControl_->regionAStatusControl()->value() == BioXASSSRLMonochromator::Region::NotIn){
			regionAStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
			regionAStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOn.png"));

		} else {
			regionAStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
			regionAStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
		}

	} else {
		regionAStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
		regionAStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
	}
}

void BioXASSSRLMonochromatorRegionControlView::onRegionBStatusChanged()
{
	if (regionControl_ && regionControl_->regionBStatusControl() && regionControl_->regionBStatusControl()->isConnected()) {
		if (regionControl_->regionBStatusControl()->value() == BioXASSSRLMonochromator::Region::In) {
			regionBStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
			regionBStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

		} else if (regionControl_->regionBStatusControl()->value() == BioXASSSRLMonochromator::Region::NotIn){
			regionBStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
			regionBStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOn.png"));

		} else {
			regionBStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
			regionBStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
		}

	} else {
		regionBStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
		regionBStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
	}
}
