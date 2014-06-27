#include "VESPERSScalerView.h"

#include "beamline/VESPERS/VESPERSBeamline.h"

#include <QVBoxLayout>

 VESPERSScalerView::~VESPERSScalerView(){}
VESPERSScalerView::VESPERSScalerView(QWidget *parent)
	: QWidget(parent)
{
	CLSSIS3820Scaler *scaler = VESPERSBeamline::vespers()->scaler();

	QVBoxLayout *layout = new QVBoxLayout;

	compositeView_ = new CLSSIS3820CompositeScalerChannelView(scaler->channelAt(5), scaler->channelAt(6));
	compositeView_->setEnableCheckBoxVisibility(false);
	compositeView_->setCustomCompositeName("Split");
	compositeView_->setFixedHeight(55);
	compositeView_->setFixedWidth(300);
    connect(compositeView_, SIGNAL(amplifierViewModeChanged(AMCurrentAmplifierView::ViewMode)), this, SLOT(onSR570ViewChanged(AMCurrentAmplifierView::ViewMode)));
	connect(compositeView_, SIGNAL(outputViewModeChanged(CLSSIS3820CompositeScalerChannelView::OutputViewMode)), this, SLOT(onOutputViewModeChanged(CLSSIS3820CompositeScalerChannelView::OutputViewMode)));
	layout->addWidget(compositeView_);

	for (int i = 7; i <= 9; i++){

		CLSSIS3820ScalerChannelView *view = new CLSSIS3820ScalerChannelView(scaler->channelAt(i));
		view->setEnableCheckBoxVisibility(false);
		singleViews_ << view;
		view->setFixedHeight(55);
        connect(view, SIGNAL(amplifierViewModeChanged(AMCurrentAmplifierView::ViewMode)), this, SLOT(onSR570ViewChanged(AMCurrentAmplifierView::ViewMode)) );
        connect(view, SIGNAL(outputViewModeChanged(CLSSIS3820ScalerChannelView::OutputViewMode)), this, SLOT(onOutputViewModeChanged(CLSSIS3820ScalerChannelView::OutputViewMode)));
		layout->addWidget(view);
	}

	setLayout(layout);
}

void VESPERSScalerView::onSR570ViewChanged(AMCurrentAmplifierView::ViewMode mode)
{
	compositeView_->blockSignals(true);
    compositeView_->setAmplifierViewMode(mode);
	compositeView_->blockSignals(false);

    foreach (CLSSIS3820ScalerChannelView *channel, singleViews_){

        channel->blockSignals(true);
        channel->setAmplifierViewMode(mode);
        channel->blockSignals(false);
    }
}

//void VESPERSScalerView::onSR570ViewChanged(AMCurrentAmplifierView::ViewMode mode)
//{
//	compositeView_->blockSignals(true);
//    compositeView_->setAmplifierViewMode(mode);
//	compositeView_->blockSignals(false);

//    foreach (CLSSIS3820ScalerChannelView *channel, singleViews_){

//        channel->blockSignals(true);
//        channel->setAmplifierViewMode((AMCurrentAmplifierView::ViewMode)mode);
//        channel->blockSignals(false);
//    }
//}

void VESPERSScalerView::onOutputViewModeChanged(CLSSIS3820CompositeScalerChannelView::OutputViewMode mode)
{
	compositeView_->blockSignals(true);
	compositeView_->setOutputViewMode(mode);
	compositeView_->blockSignals(false);

	foreach (CLSSIS3820ScalerChannelView *channel, singleViews_){

		channel->blockSignals(true);
		channel->setOutputViewMode((CLSSIS3820ScalerChannelView::OutputViewMode)mode);
		channel->blockSignals(false);
	}
}

void VESPERSScalerView::onOutputViewModeChanged(CLSSIS3820ScalerChannelView::OutputViewMode mode)
{
	compositeView_->blockSignals(true);
	compositeView_->setOutputViewMode((CLSSIS3820CompositeScalerChannelView::OutputViewMode)mode);
	compositeView_->blockSignals(false);

	foreach (CLSSIS3820ScalerChannelView *channel, singleViews_){

		channel->blockSignals(true);
		channel->setOutputViewMode(mode);
		channel->blockSignals(false);
	}
}
