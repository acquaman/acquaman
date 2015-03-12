#include "BioXASMainMonochromatorRegionView.h"

BioXASMainMonochromatorRegionView::BioXASMainMonochromatorRegionView(QWidget *parent) :
	QGroupBox(parent)
{
	// Initialize member variables.

	mono_ = BioXASMainBeamline::bioXAS()->mono();

	// Create UI elements.

	QLabel *slitsStatusPrompt = new QLabel("Slits status:");
	slitsStatusGreen_ = new QLabel();
	slitsStatusRed_ = new QLabel();

	QLabel *paddleStatusPrompt = new QLabel ("Paddle status:");
	paddleStatusGreen_ = new QLabel();
	paddleStatusRed_ = new QLabel();

	QLabel *keyStatusPrompt = new QLabel("Key status:");
	keyStatusGreen_ = new QLabel();
	keyStatusRed_ = new QLabel();

	QLabel *brakeStatusPrompt = new QLabel("Brake status:");
	brakeStatusGreen_ = new QLabel();
	brakeStatusRed_ = new QLabel();

//	QLabel *crystal1StatusPrompt = new QLabel("Crystal 1 status:");
//	crystal1StatusGreen_ = new QLabel();
//	crystal1StatusRed_ = new QLabel();

//	QLabel *crystal2StatusPrompt = new QLabel("Crystal 2 status:");
//	crystal2StatusGreen_ = new QLabel();
//	crystal2StatusRed_ = new QLabel();

	// Create and set layouts.

	QGridLayout *statusLayout = new QGridLayout();
//	statusLayout->setMargin(0);
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
//	statusLayout->addWidget(crystal1StatusPrompt, 4, 0, 1, 1, Qt::AlignRight);
//	statusLayout->addWidget(crystal1StatusGreen_, 4, 1, 1, 1, Qt::AlignCenter);
//	statusLayout->addWidget(crystal1StatusRed_, 4, 2, 1, 1, Qt::AlignCenter);
//	statusLayout->addWidget(crystal2StatusPrompt, 5, 0, 1, 1, Qt::AlignRight);
//	statusLayout->addWidget(crystal2StatusGreen_, 5, 1, 1, 1, Qt::AlignCenter);
//	statusLayout->addWidget(crystal2StatusRed_, 5, 2, 1, 1, Qt::AlignCenter);

	setLayout(statusLayout);

	// Make connections.

	connect( mono_, SIGNAL(slitsStatusChanged(bool)), this, SLOT(onSlitsStatusChanged()) );
	connect( mono_, SIGNAL(paddleStatusChanged(bool)), this, SLOT(onPaddleStatusChanged()) );
	connect( mono_, SIGNAL(keyStatusChanged(bool)), this, SLOT(onKeyStatusChanged()) );
	connect( mono_, SIGNAL(brakeStatusChanged(bool)), this, SLOT(onBrakeStatusChanged()) );
//	connect( mono_->crystalChangeMotor()->cwLimitControl(), SIGNAL(valueChanged(double)), this, SLOT(onCWLimitStatusChanged()) );
//	connect( mono_->crystalChangeMotor()->ccwLimitControl(), SIGNAL(valueChanged(double)), this, SLOT(onCCWLimitStatusChanged()) );

	// Current settings.

	setFlat(true);

	onSlitsStatusChanged();
	onPaddleStatusChanged();
	onKeyStatusChanged();
	onBrakeStatusChanged();
//	onCWLimitStatusChanged();
//	onCCWLimitStatusChanged();
}

BioXASMainMonochromatorRegionView::~BioXASMainMonochromatorRegionView()
{

}

void BioXASMainMonochromatorRegionView::onSlitsStatusChanged()
{
	if (mono_->slitsStatusControl()->isConnected()) {
		if (mono_->slitsStatus() == BioXASSSRLMonochromator::Slits::Closed) {
			slitsStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
			slitsStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

		} else {
			slitsStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
			slitsStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOn.png"));
		}

	} else {
		slitsStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
		slitsStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
	}
}

void BioXASMainMonochromatorRegionView::onPaddleStatusChanged()
{
	if (mono_->paddleStatusControl()->isConnected()) {
		if (mono_->paddleStatus() == BioXASSSRLMonochromator::Paddle::Out) {
			paddleStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
			paddleStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

		} else {
			paddleStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
			paddleStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOn.png"));
		}

	} else {
		paddleStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
		paddleStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
	}
}

void BioXASMainMonochromatorRegionView::onKeyStatusChanged()
{
	if (mono_->keyStatusControl()->isConnected()) {
		if (mono_->keyStatus() == BioXASSSRLMonochromator::Key::Enabled) {
			keyStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
			keyStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

		} else if (mono_->keyStatus() == BioXASSSRLMonochromator::Key::Disabled){
			keyStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
			keyStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOn.png"));

		} else {
			keyStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
			keyStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
		}
	}
}

void BioXASMainMonochromatorRegionView::onBrakeStatusChanged()
{
	if (mono_->brakeStatus() == BioXASSSRLMonochromator::Brake::Enabled) {
		brakeStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
		brakeStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

	} else if (mono_->brakeStatus() == BioXASSSRLMonochromator::Brake::Disabled){
		brakeStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
		brakeStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOn.png"));

	} else {
		brakeStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
		brakeStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
	}
}

//void BioXASMainMonochromatorRegionView::onCWLimitStatusChanged()
//{
//	if (mono_->crystalChangeMotor()->cwLimitControl()->isConnected()) {
//		qDebug() << "Crystal change motor cw limit is connected.";

//		double value = mono_->crystalChangeMotor()->cwLimitControl()->value();

//		if (value == 1) {
//			qDebug() << "Crystal change motor is at cw limit.";

//			crystal1StatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
//			crystal1StatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

//		} else {
//			qDebug() << "Crystal change motor is not at cw limit.";

//			crystal1StatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
//			crystal1StatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
//		}

//	} else {
//		qDebug() << "Crystal change motor cw limit is not connected.";

//		crystal1StatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
//		crystal1StatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
//	}
//}

//void BioXASMainMonochromatorRegionView::onCCWLimitStatusChanged()
//{
//	if (mono_->crystalChangeMotor()->ccwLimitControl()->isConnected()) {
//		qDebug() << "Crystal change motor ccw limit is connected.";

//		double value = mono_->crystalChangeMotor()->cwLimitControl()->value();

//		if (value == 1) {
//			qDebug() << "Crystal change motor is at ccw limit.";

//			crystal2StatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
//			crystal2StatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

//		} else {
//			qDebug() << "Crystal change motor is not at ccw limit.";

//			crystal2StatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
//			crystal2StatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
//		}

//	} else {
//		qDebug() << "Crystal change motor ccw limit is not connected.";

//		crystal2StatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
//		crystal2StatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
//	}
//}
