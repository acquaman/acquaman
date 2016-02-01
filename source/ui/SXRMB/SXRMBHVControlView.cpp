#include "SXRMBHVControlView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDoubleSpinBox>


SXRMBHVControlView::SXRMBHVControlView(AMControlSet * hvControlSet, bool viewOnly, QWidget *parent) :
    QWidget(parent)
{
	hvControlSet_ = hvControlSet;

	QVBoxLayout *contentLayout = new QVBoxLayout;
	contentLayout->setContentsMargins(1, 1, 1, 1);
	setLayout(contentLayout);

	for (int i = 0; i < hvControlSet_->count(); i++) {
		SXRMBHVControl *hvControl = qobject_cast<SXRMBHVControl *>(hvControlSet_->at(i));
		SXRMBHVControlChannelView *hvControlChannelView = new SXRMBHVControlChannelView(hvControl, viewOnly);
		contentLayout->addWidget(hvControlChannelView);
	}
}


//=================== HV control channel ====================
SXRMBHVControlChannelView::SXRMBHVControlChannelView(SXRMBHVControl * hvControl, bool viewOnly, QWidget *parent) :
	QWidget(parent)
{
	hvControl_ = hvControl;

	contentLayout_ = new QHBoxLayout;
	contentLayout_->setContentsMargins(1, 5, 1, 5);
	setLayout(contentLayout_);

	layoutHVControl(hvControl, viewOnly);
	setupConnections();
}


void SXRMBHVControlChannelView::onHVControlStatusChanged(int status)
{
	if (status == 1) {
		statusLabel_->setPixmap(QIcon(":/32x32/greenLEDOn.png").pixmap(22));

		powerOnOffButton_->setText("Turn Off");
		disconnect(powerOnOffButton_, SIGNAL(clicked()), hvControl_, SLOT(onPowerOn()));
		connect(powerOnOffButton_, SIGNAL(clicked()), hvControl_, SLOT(onPowerOff()));

		if (voltageSpinBox_) {
			voltageSpinBox_->setEnabled(true);
		}
	} else {
		statusLabel_->setPixmap(QIcon(":/32x32/greenLEDOff.png").pixmap(22));

		powerOnOffButton_->setText("Turn On");
		disconnect(powerOnOffButton_, SIGNAL(clicked()), hvControl_, SLOT(onPowerOff()));
		connect(powerOnOffButton_, SIGNAL(clicked()), hvControl_, SLOT(onPowerOn()));

		if (voltageSpinBox_) {
			voltageSpinBox_->setEnabled(false);
		}
	}
}

void SXRMBHVControlChannelView::onHVControlMeasuredCurrentValueChanged(double value)
{
	measuredCurrentLabel_->setText(QString("%1 uA").arg(value));
}

void SXRMBHVControlChannelView::onHVControlVoltageValueChanged(double value)
{
	if (actVoltageLabel_)
		actVoltageLabel_->setText(QString("%1 V").arg(value));
	else if (voltageSpinBox_)
		voltageSpinBox_->setValue(value);
}

void SXRMBHVControlChannelView::onVoltageSpinBoxEditFinished()
{
	double voltage = voltageSpinBox_->value();
	hvControl_->move(voltage);
}

void SXRMBHVControlChannelView::layoutHVControl(SXRMBHVControl *hvControl, bool viewOnly)
{
	nameLabel_ = new QLabel(hvControl->name());
	nameLabel_->setAlignment(Qt::AlignCenter);
	if (viewOnly)
		nameLabel_->setMaximumWidth(150);
	else
		nameLabel_->setMinimumWidth(100);

	actVoltageLabel_ = 0;
	voltageSpinBox_ = 0;
	if (viewOnly) {
		actVoltageLabel_ = new QLabel("0.00 V");
		actVoltageLabel_->setFixedWidth(90);
		actVoltageLabel_->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	} else {
		voltageSpinBox_ = new QDoubleSpinBox();
		voltageSpinBox_->setSuffix(" V");
		voltageSpinBox_->setSingleStep(5);
		voltageSpinBox_->setRange(0, 300);
		voltageSpinBox_->setDecimals(2);
		voltageSpinBox_->setAlignment(Qt::AlignCenter);
		voltageSpinBox_->setFixedWidth(90);
	}

	measuredCurrentLabel_ = new QLabel("0.00 uA");
	measuredCurrentLabel_->setFixedWidth(90);
	measuredCurrentLabel_->setAlignment(Qt::AlignVCenter | Qt::AlignRight);

	powerOnOffButton_ = new QPushButton("Turn Off");
	powerOnOffButton_->setMaximumWidth(60);

	statusLabel_ = new QLabel;
	statusLabel_->setPixmap(QIcon(":/32x32/greenLEDOff.png").pixmap(22));

	contentLayout_->addWidget(nameLabel_, 0, Qt::AlignLeft);
	if (viewOnly)
		contentLayout_->addWidget(actVoltageLabel_);
	else
		contentLayout_->addWidget(voltageSpinBox_);
	contentLayout_->addWidget(measuredCurrentLabel_);
	contentLayout_->addWidget(powerOnOffButton_);
	contentLayout_->addWidget(statusLabel_);
}

void SXRMBHVControlChannelView::setupConnections()
{
	connect(hvControl_, SIGNAL(powerStatusChanged(int)), this, SLOT(onHVControlStatusChanged(int)));
	connect(hvControl_, SIGNAL(voltageValueChanged(double)), this, SLOT(onHVControlVoltageValueChanged(double)));
	connect(hvControl_, SIGNAL(currentValueChanged(double)), this, SLOT(onHVControlMeasuredCurrentValueChanged(double)));

	connect(powerOnOffButton_, SIGNAL(clicked()), hvControl_, SLOT(onPowerOff()));

	if (voltageSpinBox_)
		connect(voltageSpinBox_, SIGNAL(editingFinished()), this, SLOT(onVoltageSpinBoxEditFinished()));


	if (hvControl_->isConnected()) {
		onHVControlStatusChanged(hvControl_->powerStatus());
		onHVControlVoltageValueChanged(hvControl_->voltage());
		onHVControlMeasuredCurrentValueChanged(hvControl_->measuredCurrent());
	}
}
