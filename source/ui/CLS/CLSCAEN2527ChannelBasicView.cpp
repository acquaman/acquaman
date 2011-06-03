#include "CLSCAEN2527ChannelBasicView.h"

#include "QLineEdit"
#include "QVBoxLayout"
#include "ui/AMControlEditor.h"
#include "beamline/CLS/CLSCAEN2527HVChannel.h"

CLSCAEN2527ChannelBasicView::CLSCAEN2527ChannelBasicView(CLSCAEN2527HVChannel *hvChannel, QWidget *parent) :
		QGroupBox(parent)
{
	hvChannel_ = 0;
	if(hvChannel){
		hvChannel_ = hvChannel;
		setTitle(hvChannel_->name());
		descriptionEdit_ = new QLineEdit();
		demandCE_ = new AMControlEditor(hvChannel_->demandControl());
		voltageCE_ = new AMControlEditor(hvChannel_->voltageControl(), 0, true);
		toggleCB_ = new AMControlButton(hvChannel_->toggleControl());
		toggleCB_->setDownValue(1);
		toggleCB_->setUpValue(0);
		if(hvChannel_->isConnected() && hvChannel_->isOn())
			toggleCB_->setText("On");
		else if(hvChannel_->isConnected() && hvChannel_->isOff())
			toggleCB_->setText("Off");
		connect(hvChannel_, SIGNAL(connected(bool)), this, SLOT(onHVChannelConnected(bool)));
		connect(hvChannel_, SIGNAL(powerStateChanged(highVoltageChannelPowerState)), this, SLOT(onPowerStateChanged(AMHighVoltageChannel::highVoltageChannelPowerState)));
		statusCE_ = new AMControlEditor(hvChannel_->statusControl(), 0, true);
		currentCE_ = new AMControlEditor(hvChannel_->currentControl(), 0, true);

		vl_ = new QVBoxLayout();
		vl_->addWidget(descriptionEdit_);
		vl_->addWidget(demandCE_);
		vl_->addWidget(voltageCE_);
		vl_->addWidget(toggleCB_);
		vl_->addWidget(statusCE_);
		vl_->addWidget(currentCE_);
		setLayout(vl_);

		connect(hvChannel_, SIGNAL(descriptionChanged(QString)), descriptionEdit_, SLOT(setText(QString)));
		connect(descriptionEdit_, SIGNAL(textEdited(QString)), hvChannel_, SLOT(setDescription(QString)));
	}
}

void CLSCAEN2527ChannelBasicView::onHVChannelConnected(bool connect){
	Q_UNUSED(connect);
	if(hvChannel_->isConnected() && hvChannel_->isOn())
		toggleCB_->setText("On");
	else if(hvChannel_->isConnected() && hvChannel_->isOff())
		toggleCB_->setText("Off");
}

void CLSCAEN2527ChannelBasicView::onPowerStateChanged(AMHighVoltageChannel::highVoltageChannelPowerState powerState){
	if(powerState = AMHighVoltageChannel::isPowerOn)
		toggleCB_->setText("On");
	else if(powerState = AMHighVoltageChannel::isPowerOff)
		toggleCB_->setText("Off");
}
