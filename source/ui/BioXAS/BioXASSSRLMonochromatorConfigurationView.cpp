#include "BioXASSSRLMonochromatorConfigurationView.h"

BioXASSSRLMonochromatorConfigurationView::BioXASSSRLMonochromatorConfigurationView(BioXASSSRLMonochromator *mono, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	mono_ = 0;

	// Create UI elements.

	braggOffset_ = new QDoubleSpinBox();

	braggEncoderMoveType_ = new QComboBox();

	braggSettlingTime_ = new QDoubleSpinBox();

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(braggOffset_);
	layout->addWidget(braggEncoderMoveType_);
	layout->addWidget(braggSettlingTime_);

	setLayout(layout);

	// Current settings.

	setMono(mono);
}

BioXASSSRLMonochromatorConfigurationView::~BioXASSSRLMonochromatorConfigurationView()
{

}

void BioXASSSRLMonochromatorConfigurationView::setMono(BioXASSSRLMonochromator *newMono)
{
	if (mono_ != newMono_) {
		mono_ = newMono;
		emit monoChanged(mono_);
	}
}

void BioXASSSRLMonochromatorConfigurationView::onBraggMotorOffsetChanged()
{

}

void BioXASSSRLMonochromatorConfigurationView::onBraggMotorEncoderMoveTypeChanged()
{

}

void BioXASSSRLMonochromatorConfigurationView::onBraggMotorSettlingTimeChanged()
{

}
