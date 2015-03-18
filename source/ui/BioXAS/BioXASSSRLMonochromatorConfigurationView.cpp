#include "BioXASSSRLMonochromatorConfigurationView.h"

BioXASSSRLMonochromatorConfigurationView::BioXASSSRLMonochromatorConfigurationView(BioXASSSRLMonochromator *mono, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	mono_ = 0;

	// Create UI elements.

	regionEditor_ = 0;

	energyEditor_ = 0;

	braggMotorEditor_ = 0;

	QGroupBox *regionStatusView = new QGroupBox("Region status");

	regionStatusWidget_ = new BioXASSSRLMonochromatorRegionControlView(0);

	QGroupBox *braggConfigView = new QGroupBox("Bragg configuration");

	QLabel *braggOffsetPrompt = new QLabel("Offset:");

	braggOffset_ = new QDoubleSpinBox();

	QLabel *braggEncoderMoveTypePrompt = new QLabel("Encoder move type:");

	braggEncoderMoveType_ = new QComboBox();

	QLabel *braggSettlingTimePrompt = new QLabel("Settling time:");

	braggSettlingTime_ = new QDoubleSpinBox();

	// Create and set layouts.

	QVBoxLayout *regionStatusViewLayout = new QVBoxLayout();
	regionStatusViewLayout->setMargin(0);
	regionStatusViewLayout->addWidget(regionStatusWidget_);

	regionStatusView->setLayout(regionStatusViewLayout);

	QGridLayout *braggConfigViewLayout = new QGridLayout();
	braggConfigViewLayout->addWidget(braggOffsetPrompt, 0, 0, 1, 1, Qt::AlignRight);
	braggConfigViewLayout->addWidget(braggOffset_, 0, 1, 1, 1, Qt::AlignCenter);
	braggConfigViewLayout->addWidget(braggEncoderMoveTypePrompt, 1, 0, 1, 1, Qt::AlignRight);
	braggConfigViewLayout->addWidget(braggEncoderMoveType_, 1, 1, 1, 1, Qt::AlignCenter);
	braggConfigViewLayout->addWidget(braggSettlingTimePrompt, 2, 0, 1, 1, Qt::AlignRight);
	braggConfigViewLayout->addWidget(braggSettlingTime_, 2, 1, 1, 1, Qt::AlignCenter);

	braggConfigView->setLayout(braggConfigViewLayout);

	QVBoxLayout *layoutV = new QVBoxLayout();
	layoutV->addWidget(regionStatusView);
	layoutV->addWidget(braggConfigView);
	layoutV->addStretch();

	QHBoxLayout *layoutH = new QHBoxLayout();
	layoutH->addLayout(layoutV);
	layoutH->addStretch();

	setLayout(layoutH);

	// Initial settings.

	initializeUI();

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
			disconnect( mono_->braggMotor(), 0, this, 0 );
		}

		mono_ = newMono;

		if (mono_) {
			connect( mono_->braggMotor(), SIGNAL(EGUOffsetChanged(double)), this, SLOT(onBraggMotorOffsetChanged()) );
			connect( mono_->braggMotor(), SIGNAL(encoderMovementTypeChanged(CLSMAXvMotor::EncoderMovementType)), this, SLOT(onBraggMotorEncoderMoveTypeChanged()) );
		}

		refreshUI();

		emit monoChanged(mono_);
	}
}

void BioXASSSRLMonochromatorConfigurationView::onBraggMotorOffsetChanged()
{
	if (mono_ && mono_->braggMotor() && mono_->braggMotor()->isConnected()) {
		braggOffset_->setEnabled(true);
		braggOffset_->setSuffix("deg");
		braggOffset_->setMinimum(BRAGG_OFFSET_MIN);
		braggOffset_->setMaximum(BRAGG_OFFSET_MAX);
		braggOffset_->setValue(mono_->braggMotor()->EGUOffset());

	} else {
		braggOffset_->clear();
		braggOffset_->setEnabled(false);
	}
}

void BioXASSSRLMonochromatorConfigurationView::onBraggMotorEncoderMoveTypeChanged()
{
	if (mono_ && mono_->braggMotor() && mono_->braggMotor()->isConnected()) {
		braggEncoderMoveType_->setEnabled(true);
		braggEncoderMoveType_->addItem("Off");
		braggEncoderMoveType_->addItem("Increase Only");
		braggEncoderMoveType_->addItem("Decrease Only");
		braggEncoderMoveType_->addItem("Increase");
		braggEncoderMoveType_->addItem("Decrease");
		braggEncoderMoveType_->setCurrentIndex(mono_->braggMotor()->encoderMovementType());

	} else {
		braggEncoderMoveType_->clear();
		braggEncoderMoveType_->setEnabled(false);
	}
}

void BioXASSSRLMonochromatorConfigurationView::onBraggMotorSettlingTimeChanged()
{
	braggSettlingTime_->clear();
	braggSettlingTime_->setEnabled(false);
}

void BioXASSSRLMonochromatorConfigurationView::clearUI()
{
	braggOffset_->clear();
	braggOffset_->setEnabled(false);

	braggEncoderMoveType_->clear();
	braggEncoderMoveType_->setEnabled(false);

	braggSettlingTime_->clear();
	braggSettlingTime_->setEnabled(false);
}

void BioXASSSRLMonochromatorConfigurationView::initializeUI()
{
	if (mono_) {
		regionStatusWidget_->setRegionControl(mono_->regionControl());

	} else {
		regionStatusWidget_->setRegionControl(0);
	}

	onBraggMotorOffsetChanged();
	onBraggMotorEncoderMoveTypeChanged();
	onBraggMotorSettlingTimeChanged();
}

void BioXASSSRLMonochromatorConfigurationView::refreshUI()
{
	clearUI();
	initializeUI();
}
