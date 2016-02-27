#include "BioXASSIS3820ScalerChannelsView.h"

BioXASSIS3820ScalerChannelsView::BioXASSIS3820ScalerChannelsView(CLSSIS3820Scaler *scaler, bool biasEnabledVisible, bool biasVisible, bool darkCurrentVisible, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	scaler_ = scaler;
	biasEnabledEditorVisible_ = false;
	biasEditorVisible_ = false;
	darkCurrentVisible_ = false;

	// Create UI elements.

	int channelCount = scaler_->channels().count();

	if (scaler_) {
		for (int i = 0; i < channelCount; i++) {
			CLSSIS3820ScalerChannel *channel = scaler_->channelAt(i);

			if (channel) {
				BioXASSIS3820ScalerChannelView *channelView = new BioXASSIS3820ScalerChannelView(channel, biasEnabledEditorVisible_, biasEditorVisible_);
				channelViews_ << channelView;

				channelView->setAmplifierViewFormat('e');
				channelView->setVisible(!channel->customChannelName().isEmpty());

				connect( channelView, SIGNAL(amplifierViewModeChanged(AMCurrentAmplifierView::ViewMode)), this, SLOT(setAmplifierViewMode(AMCurrentAmplifierView::ViewMode)) );
				connect( channelView, SIGNAL(outputViewModeChanged(CLSSIS3820ScalerChannelView::OutputViewMode)), this, SLOT(setOutputViewMode(CLSSIS3820ScalerChannelView::OutputViewMode)) );
			}
		}
	}

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);

	for (int i = 0, viewCount = channelViews_.count(); i < viewCount; i++) {
		BioXASSIS3820ScalerChannelView *channelView = channelViews_.at(i);
		layout->addWidget(channelView);
	}

	setLayout(layout);

	// Connect to channels.

	for (int i = 0; i < channelCount; i++) {
		CLSSIS3820ScalerChannel *channel = scaler_->channelAt(i);

		connect( channel, SIGNAL(customNameChanged(QString)), this, SLOT(refresh()) );
		connect( channel, SIGNAL(detectorChanged(AMDetector*)), this, SLOT(refresh()) );
	}

	// Current settings.

	refresh();

	setBiasEnabledEditorVisible(biasEnabledVisible);
	setBiasEditorVisible(biasVisible);
	setDarkCurrentVisible(darkCurrentVisible);
}

BioXASSIS3820ScalerChannelsView::~BioXASSIS3820ScalerChannelsView()
{

}

void BioXASSIS3820ScalerChannelsView::refresh()
{
	// Update channel views visibility.

	for (int i = 0, channelCount = scaler_->channels().count(); i < channelCount; i++) {
		channelViews_.at(i)->setVisible(!scaler_->channelAt(i)->customChannelName().isEmpty());
	}

	// Update channel views bias enabled editor.

	for (int i = 0, size = channelViews_.size(); i < size; i++) {
		channelViews_.at(i)->setBiasEnabledEditorVisible(biasEnabledEditorVisible_);
	}

	// Update channel views bias editor.

	for (int i = 0, size = channelViews_.size(); i < size; i++) {
		channelViews_.at(i)->setBiasEditorVisible(biasEditorVisible_);
	}

	// Update channel views dark current.

	for (int i = 0, size = channelViews_.size(); i < size; i++) {
		channelViews_.at(i)->setDarkCurrentVisible(darkCurrentVisible_);
	}
}

void BioXASSIS3820ScalerChannelsView::setBiasEnabledEditorVisible(bool isVisible)
{
	if (biasEnabledEditorVisible_ != isVisible) {
		biasEnabledEditorVisible_ = isVisible;
		refresh();

		emit biasEnabledEditorVisibleChanged(biasEnabledEditorVisible_);
	}
}

void BioXASSIS3820ScalerChannelsView::setBiasEditorVisible(bool isVisible)
{
	if (biasEditorVisible_ != isVisible) {
		biasEditorVisible_ = isVisible;
		refresh();

		emit biasEditorVisibleChanged(biasEditorVisible_);
	}
}

void BioXASSIS3820ScalerChannelsView::setDarkCurrentVisible(bool isVisible)
{
	if (darkCurrentVisible_ != isVisible) {
		darkCurrentVisible_ = isVisible;
		refresh();

		emit darkCurrentVisibleChanged(darkCurrentVisible_);
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
