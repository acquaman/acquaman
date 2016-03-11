#include "BioXASSIS3820ScalerChannelView.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/CLS/CLSKeithley428.h"
#include "ui/beamline/AMExtendedControlEditor.h"

BioXASSIS3820ScalerChannelView::BioXASSIS3820ScalerChannelView(CLSSIS3820ScalerChannel *channel, bool biasEnabledVisible, bool biasVisible, bool darkCurrentVisible, QWidget *parent) :
	CLSSIS3820ScalerChannelView(channel, parent)
{
	// Initialize member variables.

	detector_ = 0;

	biasEnabledVisible_ = false;
	biasVisible_ = false;
	darkCurrentVisible_ = false;

	biasEnabledEditor_ = 0;
	biasEditor_ = 0;
	darkCurrentDisplay_ = 0;

	// CLS scaler channel settings.

	scalerOutput_->setFixedWidth(100);
	channelName_->setAlignment(Qt::AlignCenter);

	// Create Keithley 428 UI elements, add to main channel layout.

	QHBoxLayout *keithleyLayout = new QHBoxLayout();
	keithleyLayout->setMargin(0);

	if (channel_->currentAmplifier()) {

		CLSKeithley428 *keithley = qobject_cast<CLSKeithley428*>(channel_->currentAmplifier());

		if (keithley) {

			biasEnabledEditor_ = new AMExtendedControlEditor(keithley->biasVoltageEnabledControl());
			biasEnabledEditor_->setNoUnitsBox(true);
			biasEnabledEditor_->setTitle("Bias enabled");
			biasEnabledEditor_->setFixedWidth(100);
			biasEnabledEditor_->hide();

			biasEditor_ = new AMExtendedControlEditor(keithley->biasVoltageControl());
			biasEditor_->setTitle("Bias");
			biasEditor_->setUnits("V");
			biasEditor_->setFixedWidth(100);
			biasEditor_->hide();

			keithleyLayout->addWidget(biasEnabledEditor_);
			keithleyLayout->addWidget(biasEditor_);
		}
	}

	channelLayout_->addLayout(keithleyLayout);

	// Create dark current UI elements.

	darkCurrentDisplay_ = new QLabel();
	darkCurrentDisplay_->setFixedWidth(150);
	darkCurrentDisplay_->setAlignment(Qt::AlignCenter);
	darkCurrentDisplay_->hide();

	QHBoxLayout *darkCurrentLayout = new QHBoxLayout();
	darkCurrentLayout->setMargin(0);
	darkCurrentLayout->addWidget(darkCurrentDisplay_, Qt::AlignCenter);

	channelLayout_->addLayout(darkCurrentLayout);

	// Make connections.

	connect( channel_, SIGNAL(detectorChanged(AMDetector*)), this, SLOT(setDetector(AMDetector*)) );

	// Current settings.

	setBiasEnabledEditorVisible(biasEnabledVisible);
	setBiasEditorVisible(biasVisible);
	setDarkCurrentVisible(darkCurrentVisible);

	setDetector(channel_->detector());
}

BioXASSIS3820ScalerChannelView::~BioXASSIS3820ScalerChannelView()
{

}

void BioXASSIS3820ScalerChannelView::setBiasEnabledEditorVisible(bool isVisible)
{
	// If the scaler channel given in the constructor is invalid or doesn't have a Keithley amplifier,
	// then the bias enabled editor will never have been instantiated. Must check whether it's possible
	// to show the editor, as well as whether the 'isVisible' arg is different than the current state.

	if (biasEnabledEditor_ && biasEnabledVisible_ != isVisible)
		biasEnabledEditor_->setVisible(biasEnabledVisible_ = isVisible);
}

void BioXASSIS3820ScalerChannelView::setBiasEditorVisible(bool isVisible)
{
	// If the scaler channel given in the constructor is invalid or doesn't have a Keithley amplifier,
	// then the bias editor will never have been instantiated. Must check whether it's possible
	// to show the editor, as well as whether the 'isVisible' arg is different than the current state.

	if (biasEditor_ && biasVisible_ != isVisible)
		biasEditor_->setVisible(biasVisible_ = isVisible);
}

void BioXASSIS3820ScalerChannelView::setDarkCurrentVisible(bool isVisible)
{
	if (darkCurrentVisible_ != isVisible)
		darkCurrentDisplay_->setVisible(darkCurrentVisible_ = isVisible);
}

void BioXASSIS3820ScalerChannelView::setDarkCurrentValue(double newValue)
{
	darkCurrentDisplay_->setText(QString("%1 counts/sec").arg(newValue));
}

void BioXASSIS3820ScalerChannelView::setDarkCurrentState(bool valid)
{
	if (valid)
		darkCurrentDisplay_->setStyleSheet("color: black;");
	else
		darkCurrentDisplay_->setStyleSheet("color: red;");
}

void BioXASSIS3820ScalerChannelView::setDetector(AMDetector *newDetector)
{
	if (detector_ != newDetector) {

		if (detector_)
			disconnect( detector_, 0, this, 0 );

		detector_ = newDetector;

		if (detector_) {
			connect( detector_, SIGNAL(darkCurrentValueChanged(double)), this, SLOT(updateDarkCurrentDisplay()) );
			connect( detector_, SIGNAL(darkCurrentValidStateChanged(bool)), this, SLOT(updateDarkCurrentDisplay()) );
		}

		updateDarkCurrentDisplay();
	}
}

void BioXASSIS3820ScalerChannelView::updateDarkCurrentDisplay()
{
	if (detector_) {
		setDarkCurrentValue(detector_->darkCurrentValue());
		setDarkCurrentState(detector_->darkCurrentValidState());
	}
}
