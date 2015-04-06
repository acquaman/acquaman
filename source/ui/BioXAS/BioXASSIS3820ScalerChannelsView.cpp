#include "BioXASSIS3820ScalerChannelsView.h"

BioXASSIS3820ScalerChannelsView::BioXASSIS3820ScalerChannelsView(CLSSIS3820Scaler *scaler, bool biasEnabledShown, bool biasShown, bool darkCurrentShown, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	scaler_ = scaler;
	biasEnabledEditorShown_ = false;
	biasEditorShown_ = false;
	darkCurrentShown_ = false;

	// Create UI elements.

	if (scaler_) {
		for (int i = 0, size = scaler_->channels().count(); i < size; i++) {
			CLSSIS3820ScalerChannel *channel = scaler_->channelAt(i);

			if (channel && !channel->customChannelName().isEmpty()) {
				BioXASSIS3820ScalerChannelView *channelView = new BioXASSIS3820ScalerChannelView(channel, biasEnabledEditorShown_, biasEditorShown_);
				channelViews_ << channelView;

				connect( channelView, SIGNAL(amplifierViewModeChanged(AMCurrentAmplifierView::ViewMode)), this, SLOT(setAmplifierViewMode(AMCurrentAmplifierView::ViewMode)) );
				connect( channelView, SIGNAL(outputViewModeChanged(CLSSIS3820ScalerChannelView::OutputViewMode)), this, SLOT(setOutputViewMode(CLSSIS3820ScalerChannelView::OutputViewMode)) );
			}
		}
	}

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);

	foreach (BioXASSIS3820ScalerChannelView *channelView, channelViews_) {
		layout->addWidget(channelView);
	}

	setLayout(layout);

	// Current settings.

	setBiasEnabledEditorShown(biasEnabledShown);
	setBiasEditorShown(biasShown);
	setDarkCurrentShown(darkCurrentShown);
}

BioXASSIS3820ScalerChannelsView::~BioXASSIS3820ScalerChannelsView()
{

}

void BioXASSIS3820ScalerChannelsView::setBiasEnabledEditorShown(bool shown)
{
	if (biasEnabledEditorShown_ != shown) {
		biasEnabledEditorShown_ = shown;

		for (int i = 0, size = channelViews_.size(); i < size; i++) {
			channelViews_.at(i)->setBiasEnabledEditorShown(biasEnabledEditorShown_);
		}

		emit biasEnabledEditorShownChanged(biasEnabledEditorShown_);
	}
}

void BioXASSIS3820ScalerChannelsView::setBiasEditorShown(bool shown)
{
	if (biasEditorShown_ != shown) {
		biasEditorShown_ = shown;

		for (int i = 0, size = channelViews_.size(); i < size; i++) {
			channelViews_.at(i)->setBiasEditorShown(biasEditorShown_);
		}

		emit biasEditorShownChanged(biasEditorShown_);
	}
}

void BioXASSIS3820ScalerChannelsView::setDarkCurrentShown(bool shown)
{
	if (darkCurrentShown_ != shown) {
		darkCurrentShown_ = shown;

		for (int i = 0, size = channelViews_.size(); i < size; i++) {
			channelViews_.at(i)->setDarkCurrentShown(darkCurrentShown_);
		}

		emit darkCurrentShownChanged(darkCurrentShown_);
	}
}

void BioXASSIS3820ScalerChannelsView::setAmplifierViewMode(AMCurrentAmplifierView::ViewMode newMode)
{
	foreach (BioXASSIS3820ScalerChannelView *channelView, channelViews_) {
		channelView->blockSignals(true);
		channelView->setAmplifierViewMode(newMode);
		channelView->blockSignals(false);
	}
}

void BioXASSIS3820ScalerChannelsView::setOutputViewMode(CLSSIS3820ScalerChannelView::OutputViewMode newMode)
{
	foreach (BioXASSIS3820ScalerChannelView *channelView, channelViews_) {
		channelView->blockSignals(true);
		channelView->setOutputViewMode(newMode);
		channelView->blockSignals(false);
	}
}
