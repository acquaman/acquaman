#include "BioXASSideScalerView.h"

BioXASSideScalerView::BioXASSideScalerView(CLSSIS3820Scaler *scaler, QWidget *parent) :
    QWidget(parent)
{
    scaler_ = scaler;

    if (scaler_) {

        QVBoxLayout *layout = new QVBoxLayout();

        // Create scaler channel views.

        for (int i = 0; i < 2; i++) {

            CLSSIS3820ScalerChannelView *view = new CLSSIS3820ScalerChannelView(scaler_->channelAt(i), this);
            view->setEnableCheckBoxVisibility(false);
            view->setAmplifierViewFormat('g');
            view->setAmplifierViewPrecision(3);
            view->setFixedHeight(55);
            view->setMinimumWidth(300);

            channelViews_ << view;

            connect( view, SIGNAL(amplifierViewModeChanged(AMCurrentAmplifierView::ViewMode)), this, SLOT(onAmplifierViewModeChanged(AMCurrentAmplifierView::ViewMode)) );
            connect( view, SIGNAL(outputViewModeChanged(CLSSIS3820ScalerChannelView::OutputViewMode)), this, SLOT(onOutputViewModeChanged(CLSSIS3820ScalerChannelView::OutputViewMode)) );

            layout->addWidget(view);
        }

        setLayout(layout);
    }
}

BioXASSideScalerView::~BioXASSideScalerView()
{

}

void BioXASSideScalerView::onAmplifierViewModeChanged(AMCurrentAmplifierView::ViewMode newMode)
{
    foreach (CLSSIS3820ScalerChannelView *channel, channelViews_) {
        channel->blockSignals(true);
        channel->setAmplifierViewMode(newMode);
        channel->blockSignals(false);
    }
}

void BioXASSideScalerView::onOutputViewModeChanged(CLSSIS3820ScalerChannelView::OutputViewMode newMode)
{
    foreach (CLSSIS3820ScalerChannelView *channel, channelViews_) {
        channel->blockSignals(true);
        channel->setOutputViewMode(newMode);
        channel->blockSignals(false);
    }
}
