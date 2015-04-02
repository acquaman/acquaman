#include "BioXASSIS3820ScalerView.h"
#include "ui/BioXAS/BioXASSIS3820ScalerChannelView.h"

BioXASSIS3820ScalerView::BioXASSIS3820ScalerView(CLSSIS3820Scaler *scaler, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	scaler_ = scaler;

	// Create UI elements.

	controlsView_ = new CLSSIS3820ScalerControlsView(scaler_);

	QVBoxLayout *channelsLayout = new QVBoxLayout();

	if (scaler_) {
		for (int i = 0; i < scaler_->channels().count(); i++) {
			CLSSIS3820ScalerChannel *channel = scaler_->channelAt(i);

			if (channel && !channel->customChannelName().isEmpty()) {
				BioXASSIS3820ScalerChannelView *channelView = new BioXASSIS3820ScalerChannelView(scaler_->channelAt(i));
				channelsLayout->addWidget(channelView);
			}
		}
	}

	channelsView_ = new QGroupBox();
	channelsView_->setLayout(channelsLayout);

	// Create and set main layout.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(controlsView_);
	layout->addWidget(channelsView_);

	setLayout(layout);

	// Initial settings.

	channelsView_->hide();

	// Make connections.

	connect( scaler_, SIGNAL(connectedChanged(bool)), this, SLOT(onScalerConnected()) );

	// Current settings.

	onScalerConnectedChanged();
}

BioXASSIS3820ScalerView::~BioXASSIS3820ScalerView()
{

}

void BioXASSIS3820ScalerView::onScalerConnectedChanged()
{
	if (scaler_ && scaler_->isConnected()) {
		channelsView_->show();

	} else {
		channelsView_->hide();
	}
}
