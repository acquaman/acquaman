#include "BioXASSIS3820ScalerChannelView.h"
#include "beamline/CLS/CLSKeithley428.h"
#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/CLS/CLSSIS3820ScalerView.h"

BioXASSIS3820ScalerChannelView::BioXASSIS3820ScalerChannelView(CLSSIS3820ScalerChannel *channel, QWidget *parent) :
	QWidget(parent)
{
	// Initialize member variables.

	channel_ = channel;

	// Create UI elements and sub-layouts.

	QHBoxLayout *keithleyLayout = new QHBoxLayout();
	QHBoxLayout *channelLayout = new QHBoxLayout();

	if (channel_ && channel_->currentAmplifier()) {

		// Create scaler channel UI elements.

		CLSSIS3820ScalerChannelView *channelView = new CLSSIS3820ScalerChannelView(channel_);

		channelLayout->addWidget(channelView);
		channelLayout->addStretch();

		// Create Keithley 428 UI elements.

		CLSKeithley428 *keithley = qobject_cast<CLSKeithley428*>(channel_->currentAmplifier());

		if (keithley) {

			AMExtendedControlEditor *biasEnabledEditor = new AMExtendedControlEditor(keithley->biasVoltageEnabledControl());
			AMExtendedControlEditor *biasEditor = new AMExtendedControlEditor(keithley->biasVoltageControl());

			keithleyLayout->addWidget(biasEnabledEditor);
			keithleyLayout->addWidget(biasEditor);
		}
	}

	// Create and set main layout.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addLayout(channelLayout);
	layout->addLayout(keithleyLayout);

	setLayout(layout);
}

BioXASSIS3820ScalerChannelView::~BioXASSIS3820ScalerChannelView()
{

}
