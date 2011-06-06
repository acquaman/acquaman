#include "CLSCAEN2527ChannelBasicView.h"

#include "QLineEdit"
#include "QPushButton"
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
		toggleButton_ = new QPushButton();
		toggleButton_->setCheckable(true);
		if(hvChannel_->isConnected() && hvChannel_->isOn())
			toggleButton_->setText("On");
		else if(hvChannel_->isConnected() && hvChannel_->isOff())
			toggleButton_->setText("Off");

		connect(hvChannel_, SIGNAL(connected(bool)), this, SLOT(onHVChannelConnected(bool)));
		connect(hvChannel_, SIGNAL(powerStateChanged(AMHighVoltageChannel::highVoltageChannelPowerState)), this, SLOT(onPowerStateChanged(AMHighVoltageChannel::highVoltageChannelPowerState)));
		connect(toggleButton_, SIGNAL(toggled(bool)), this, SLOT(onToggleButtonToggled(bool)));
		statusCE_ = new AMControlEditor(hvChannel_->statusControl(), 0, true);
		currentCE_ = new AMControlEditor(hvChannel_->currentControl(), 0, true);

		vl_ = new QVBoxLayout();
		vl_->addWidget(descriptionEdit_);
		vl_->addWidget(demandCE_);
		vl_->addWidget(voltageCE_);
		vl_->addWidget(toggleButton_);
		vl_->addWidget(statusCE_);
		vl_->addWidget(currentCE_);
		setLayout(vl_);

		connect(hvChannel_, SIGNAL(descriptionChanged(QString)), descriptionEdit_, SLOT(setText(QString)));
		connect(descriptionEdit_, SIGNAL(textEdited(QString)), hvChannel_, SLOT(setDescription(QString)));
	}
}

void CLSCAEN2527ChannelBasicView::onHVChannelConnected(bool connect){
	Q_UNUSED(connect);
	if(hvChannel_->isConnected() && hvChannel_->isOn()){
		toggleButton_->setText("On");
		toggleButton_->setDown(true);
	}
	else if(hvChannel_->isConnected() && hvChannel_->isOff()){
		toggleButton_->setText("Off");
		toggleButton_->setDown(false);
	}
}

void CLSCAEN2527ChannelBasicView::onPowerStateChanged(AMHighVoltageChannel::highVoltageChannelPowerState powerState){
	if(powerState == AMHighVoltageChannel::isPowerOn){
		toggleButton_->setText("On");
		toggleButton_->setDown(true);
	}
	else if(powerState == AMHighVoltageChannel::isPowerOff){
		toggleButton_->setText("Off");
		toggleButton_->setDown(false);
	}
}

void CLSCAEN2527ChannelBasicView::onToggleButtonToggled(bool toggled){
	if(toggled)
		hvChannel_->setOn(true);
	else
		hvChannel_->setOff();
}
