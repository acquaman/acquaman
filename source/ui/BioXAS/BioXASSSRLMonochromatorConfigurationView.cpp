#include "BioXASSSRLMonochromatorConfigurationView.h"

BioXASSSRLMonochromatorConfigurationView::BioXASSSRLMonochromatorConfigurationView(BioXASSSRLMonochromator *mono, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	mono_ = 0;

	// Create UI elements.

	QGroupBox *regionStatusView = new QGroupBox("Region status");

	regionStatusWidget_ = new BioXASSSRLMonochromatorRegionControlView(0);

	QGroupBox *braggConfigView = new QGroupBox("Bragg configuration");

	braggConfigWidget_ = new BioXASSSRLMonochromatorBraggConfigurationView(0);

	// Create and set layouts.

	QVBoxLayout *regionStatusViewLayout = new QVBoxLayout();
	regionStatusViewLayout->setMargin(0);
	regionStatusViewLayout->addWidget(regionStatusWidget_);

	regionStatusView->setLayout(regionStatusViewLayout);

	QVBoxLayout *braggConfigViewLayout = new QVBoxLayout();
	braggConfigViewLayout->setMargin(0);
	braggConfigViewLayout->addWidget(braggConfigWidget_);

	braggConfigView->setLayout(braggConfigViewLayout);

	QVBoxLayout *layoutV = new QVBoxLayout();
	layoutV->addWidget(regionStatusView);
	layoutV->addWidget(braggConfigView);
	layoutV->addStretch();

	QHBoxLayout *layoutH = new QHBoxLayout();
	layoutH->addLayout(layoutV);
	layoutH->addStretch();

	setLayout(layoutH);

	// Current settings.

	setMono(mono);
}

BioXASSSRLMonochromatorConfigurationView::~BioXASSSRLMonochromatorConfigurationView()
{

}

void BioXASSSRLMonochromatorConfigurationView::setMono(BioXASSSRLMonochromator *newMono)
{
	if (mono_ != newMono) {

		if (mono_) {
			regionStatusWidget_->setRegionControl(0);
			braggConfigWidget_->setBraggMotor(0);
		}

		mono_ = newMono;

		if (mono_) {
			regionStatusWidget_->setRegionControl(mono_->regionControl());
			braggConfigWidget_->setBraggMotor(mono_->braggMotor());
		}

		emit monoChanged(mono_);
	}
}









BioXASSSRLMonochromatorBraggConfigurationView::BioXASSSRLMonochromatorBraggConfigurationView(CLSMAXvMotor *braggMotor, QWidget *parent) :
	QWidget(parent)
{
	// Initialize member variables.

	braggMotor_ = 0;

	// Create UI elements.

	QLabel *braggOffsetPrompt = new QLabel("Offset:");

	braggOffset_ = new QDoubleSpinBox();

	QLabel *braggEncoderMoveTypePrompt = new QLabel("Encoder move type:");

	braggEncoderMoveType_ = new QComboBox();

	QLabel *braggSettlingTimePrompt = new QLabel("Settling time:");

	braggSettlingTime_ = new QDoubleSpinBox();

	// Create and set layouts.

	QGridLayout *layout = new QGridLayout();
	layout->addWidget(braggOffsetPrompt, 0, 0, 1, 1, Qt::AlignRight);
	layout->addWidget(braggOffset_, 0, 1, 1, 1, Qt::AlignCenter);
	layout->addWidget(braggEncoderMoveTypePrompt, 1, 0, 1, 1, Qt::AlignRight);
	layout->addWidget(braggEncoderMoveType_, 1, 1, 1, 1, Qt::AlignCenter);
	layout->addWidget(braggSettlingTimePrompt, 2, 0, 1, 1, Qt::AlignRight);
	layout->addWidget(braggSettlingTime_, 2, 1, 1, 1, Qt::AlignCenter);

	setLayout(layout);

	// Initial settings.

	clearUI();

	// Make connections.

	connect( braggOffset_, SIGNAL(valueChanged(double)), this, SLOT(setBraggMotorOffset(double)) );
	connect( braggEncoderMoveType_, SIGNAL(currentIndexChanged(int)), this, SLOT(setBraggMotorEncoderMovementType(int)) );
	connect( braggSettlingTime_, SIGNAL(valueChanged(double)), this, SLOT(setBraggMotorSettlingTime(double)) );

	// Current settings.

	setBraggMotor(braggMotor);
}

BioXASSSRLMonochromatorBraggConfigurationView::~BioXASSSRLMonochromatorBraggConfigurationView()
{

}

void BioXASSSRLMonochromatorBraggConfigurationView::setBraggMotor(CLSMAXvMotor *newMotor)
{
	if (braggMotor_ != newMotor) {

		if (braggMotor_) {
			disconnect( braggMotor_, 0, this, 0 );
		}

		braggMotor_ = newMotor;

		if (braggMotor_) {
			connect( braggMotor_, SIGNAL(connected(bool)), this, SLOT(onBraggMotorConnectedChanged()) );
			connect( braggMotor_, SIGNAL(EGUOffsetChanged(double)), this, SLOT(onBraggMotorOffsetChanged()) );
			connect( braggMotor_, SIGNAL(encoderMovementTypeChanged(CLSMAXvMotor::EncoderMovementType)), this, SLOT(onBraggMotorEncoderMoveTypeChanged()) );
			connect( braggMotor_, SIGNAL(settlingTimeChanged(double)), this, SLOT(onBraggMotorSettlingTimeChanged()) );
		}

		onBraggMotorConnectedChanged();

		emit braggMotorChanged(braggMotor_);
	}
}

void BioXASSSRLMonochromatorBraggConfigurationView::clearUI()
{
	braggOffset_->clear();
	braggOffset_->setEnabled(false);

	braggEncoderMoveType_->clear();
	braggEncoderMoveType_->setEnabled(false);

	braggSettlingTime_->clear();
	braggSettlingTime_->setEnabled(false);
}

void BioXASSSRLMonochromatorBraggConfigurationView::initializeUI()
{
	if (braggMotor_ && braggMotor_->isConnected()) {
		braggOffset_->setEnabled(true);
		braggOffset_->setMinimum(BRAGG_OFFSET_MIN);
		braggOffset_->setMaximum(BRAGG_OFFSET_MAX);
		braggOffset_->setValue(braggMotor_->EGUOffset());

		braggEncoderMoveType_->setEnabled(true);
		braggEncoderMoveType_->addItem("Off");
		braggEncoderMoveType_->addItem("Increase Only");
		braggEncoderMoveType_->addItem("Decrease Only");
		braggEncoderMoveType_->addItem("Increase");
		braggEncoderMoveType_->addItem("Decrease");
		braggEncoderMoveType_->setCurrentIndex(braggMotor_->encoderMovementType());

		braggSettlingTime_->setEnabled(true);
		braggSettlingTime_->setMinimum(BRAGG_SETTLING_TIME_MIN);
		braggSettlingTime_->setMaximum(BRAGG_SETTLING_TIME_MAX);
		braggSettlingTime_->setValue(braggMotor_->settlingTime());
	}
}

void BioXASSSRLMonochromatorBraggConfigurationView::refreshUI()
{
	clearUI();
	initializeUI();
}

void BioXASSSRLMonochromatorBraggConfigurationView::onBraggMotorConnectedChanged()
{
	refreshUI();
}

void BioXASSSRLMonochromatorBraggConfigurationView::onBraggMotorOffsetChanged()
{
	if (braggMotor_ && braggMotor_->isConnected()) {
		braggOffset_->setValue(braggMotor_->EGUOffset());
	}
}

void BioXASSSRLMonochromatorBraggConfigurationView::onBraggMotorEncoderMoveTypeChanged()
{
	if (braggMotor_ && braggMotor_->isConnected()) {
		braggEncoderMoveType_->setCurrentIndex(braggMotor_->encoderMovementType());
	}
}

void BioXASSSRLMonochromatorBraggConfigurationView::onBraggMotorSettlingTimeChanged()
{
	if (braggMotor_ && braggMotor_->isConnected()) {
		braggSettlingTime_->setValue(braggMotor_->settlingTime());
	}
}

void BioXASSSRLMonochromatorBraggConfigurationView::setBraggMotorOffset(double newValue)
{
	if (braggMotor_) {
		braggMotor_->setEGUOffset(newValue);
	}
}

void BioXASSSRLMonochromatorBraggConfigurationView::setBraggMotorEncoderMovementType(int newType)
{
	if (braggMotor_) {
		switch (newType) {
		case CLSMAXvMotor::EncoderMovementOff:
			braggMotor_->setEncoderMovementType(CLSMAXvMotor::EncoderMovementOff);
			break;
		case CLSMAXvMotor::EncoderMovementIncreaseOnly:
			braggMotor_->setEncoderMovementType(CLSMAXvMotor::EncoderMovementIncreaseOnly);
			break;
		case CLSMAXvMotor::EncoderMovementDecreaseOnly:
			braggMotor_->setEncoderMovementType(CLSMAXvMotor::EncoderMovementDecreaseOnly);
			break;
		case CLSMAXvMotor::EncoderMovementIncrease:
			braggMotor_->setEncoderMovementType(CLSMAXvMotor::EncoderMovementIncrease);
			break;
		case CLSMAXvMotor::EncoderMovementDecrease:
			braggMotor_->setEncoderMovementType(CLSMAXvMotor::EncoderMovementDecrease);
			break;
		default:
			break;
		}
	}
}

void BioXASSSRLMonochromatorBraggConfigurationView::setBraggMotorSettlingTime(double newTime)
{
	if (braggMotor_) {
		braggMotor_->setSettlingTime(newTime);
	}
}
