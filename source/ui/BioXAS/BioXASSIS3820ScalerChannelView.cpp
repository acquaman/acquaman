#include "BioXASSIS3820ScalerChannelView.h"
#include "beamline/CLS/CLSKeithley428.h"
#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/CLS/CLSSIS3820ScalerView.h"

BioXASSIS3820ScalerChannelView::BioXASSIS3820ScalerChannelView(CLSSIS3820ScalerChannel *channel, bool biasEnabledShown, bool biasShown, bool darkCurrentShown, QWidget *parent) :
	CLSSIS3820ScalerChannelView(channel, parent)
{
	// Initialize member variables.

	biasEnabledShown_ = false;
	biasShown_ = false;
	darkCurrentShown_ = false;

	biasEnabledEditor_ = 0;
	biasEditor_ = 0;
	darkCurrentDisplay_ = 0;

	// Create UI elements and sub-layouts.

	if (channel_) {

		// Create Keithley 428 UI elements.

		QHBoxLayout *keithleyLayout = new QHBoxLayout();
		keithleyLayout->setMargin(0);

		if (channel_->currentAmplifier()) {

			CLSKeithley428 *keithley = qobject_cast<CLSKeithley428*>(channel_->currentAmplifier());

			if (keithley) {

				biasEnabledEditor_ = new AMExtendedControlEditor(keithley->biasVoltageEnabledControl());
				biasEnabledEditor_->setNoUnitsBox(true);
				biasEnabledEditor_->setTitle("Bias enabled");
				biasEnabledEditor_->setFixedWidth(100);

				biasEditor_ = new AMExtendedControlEditor(keithley->biasVoltageControl());
				biasEditor_->setTitle("Bias");
				biasEditor_->setUnits("V");
				biasEditor_->setFixedWidth(100);

				keithleyLayout->addWidget(biasEnabledEditor_);
				keithleyLayout->addWidget(biasEditor_);
			}
		}

		// Create dark current UI elements.

		darkCurrentDisplay_ = new QLabel();
		darkCurrentDisplay_->setFixedWidth(100);
		darkCurrentDisplay_->setAlignment(Qt::AlignCenter);

		QHBoxLayout *darkCurrentLayout = new QHBoxLayout();
		darkCurrentLayout->setMargin(0);
		darkCurrentLayout->addWidget(darkCurrentDisplay_, Qt::AlignCenter);

		// Add sublayouts to main layout.

		channelLayout_->addLayout(keithleyLayout);
		channelLayout_->addLayout(darkCurrentLayout);
	}


	// Initial settings.

	if (biasEnabledEditor_)
		biasEnabledEditor_->hide();

	if (biasEditor_)
		biasEditor_->hide();

	if (darkCurrentDisplay_)
		darkCurrentDisplay_->hide();

	// Make connections.

	if (channel_ && channel_->detector()) {
		connect( channel_->detector(), SIGNAL(darkCurrentValidStateChanged(bool)), this, SLOT(setDarkCurrentState(bool)) );
		connect( channel_->detector(), SIGNAL(darkCurrentValueChanged(double)), this, SLOT(setDarkCurrentValue(double)) );
	}

	// Current settings.

	setBiasEnabledEditorShown(biasEnabledShown);
	setBiasEditorShown(biasShown);
	setDarkCurrentShown(darkCurrentShown);

	if (channel_ && channel_->detector()) {
		setDarkCurrentValue(channel_->detector()->darkCurrentValue());
		setDarkCurrentState(channel_->detector()->darkCurrentValidState());
	}
}

BioXASSIS3820ScalerChannelView::~BioXASSIS3820ScalerChannelView()
{

}

void BioXASSIS3820ScalerChannelView::setBiasEnabledEditorShown(bool shown)
{
	// if the scaler channel given in the constructor is invalid or doesn't have a Keithley amplifier,
	// then the bias enabled editor will never have been instantiated. Must check whether it's possible
	// to show the editor, as well as whether the 'shown' arg is different than the current state.

	if (biasEnabledEditor_ && biasEnabledShown_ != shown) {
		biasEnabledEditor_->setShown(biasEnabledShown_ = shown);
	}
}

void BioXASSIS3820ScalerChannelView::setBiasEditorShown(bool shown)
{
	// if the scaler channel given in the constructor is invalid or doesn't have a Keithley amplifier,
	// then the bias editor will never have been instantiated. Must check whether it's possible
	// to show the editor, as well as whether the 'shown' arg is different than the current state.

	if (biasEditor_ && biasShown_ != shown) {
		biasEditor_->setShown(biasShown_ = shown);
	}
}

void BioXASSIS3820ScalerChannelView::setDarkCurrentShown(bool shown)
{
	// if the scaler channel given in the constructor is invalid or doesn't have a Keithley amplifier,
	// then the dark current display will never have been instantiated. Must check whether it's possible
	// to show the display, as well as whether the 'shown' arg is different than the current state.

	if (darkCurrentDisplay_ && darkCurrentShown_ != shown) {
		darkCurrentDisplay_->setShown(darkCurrentShown_ = shown);
	}
}

void BioXASSIS3820ScalerChannelView::setDarkCurrentValue(double newValue)
{
	if (darkCurrentDisplay_) {
		darkCurrentDisplay_->setText(QString("%1 counts").arg(newValue));
	}
}

void BioXASSIS3820ScalerChannelView::setDarkCurrentState(bool valid)
{
	if (darkCurrentDisplay_ && valid) {
		darkCurrentDisplay_->setStyleSheet("color: black;");
	} else if (darkCurrentDisplay_) {
		darkCurrentDisplay_->setStyleSheet("color: red;");
	}
}
