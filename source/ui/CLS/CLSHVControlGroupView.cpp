#include "CLSHVControlGroupView.h"

#include <QVBoxLayout>
#include <QGroupBox>


CLSHVControlGroupView::CLSHVControlGroupView(AMControlSet * hvControlSetA, AMControlSet * hvControlSetB, bool viewOnly, QWidget *parent) :
	QWidget(parent)
{
	hvControlSetA_ = hvControlSetA;
	hvControlSetB_ = hvControlSetB;

	// the main content layout
	QHBoxLayout *contentLayout = new QHBoxLayout;
	contentLayout->setContentsMargins(1, 1, 1, 1);
	setLayout(contentLayout);

	contentLayout->addWidget(layoutHVControls(hvControlSetA_, viewOnly));
	contentLayout->addWidget(layoutHVControls(hvControlSetB_, viewOnly));
}

CLSHVControlGroupView::CLSHVControlGroupView(AMControlSet * hvControlSet, bool viewOnly, QWidget *parent) :
	QWidget(parent)
{
	hvControlSetA_ = hvControlSet;
	hvControlSetB_ = 0;

	QHBoxLayout *contentLayout = new QHBoxLayout;
	contentLayout->setContentsMargins(1, 1, 1, 1);
	setLayout(contentLayout);

	contentLayout->addWidget(layoutHVControls(hvControlSetA_, viewOnly));
}

QWidget * CLSHVControlGroupView::layoutHVControls(AMControlSet *hvControlSet, bool viewOnly)
{
	QVBoxLayout *contentLayout = new QVBoxLayout;
	QGroupBox *hvControlGroupBox = new QGroupBox(hvControlSetB_->name());
	hvControlGroupBox->setLayout(contentLayout);

	for (int i = 0, count = hvControlSet->count(); i < count; i++) {
		CLSHVControl *hvControl = qobject_cast<CLSHVControl *>(hvControlSet->at(i));
		if (hvControl) {
			CLSHVControlChannelView *hvControlChannelView = new CLSHVControlChannelView(hvControl, viewOnly);
			contentLayout->addWidget(hvControlChannelView);
		}
	}

	return hvControlGroupBox;
}

// ================== implementation of CLSHVControlChannelView =====================
CLSHVControlChannelView::CLSHVControlChannelView(CLSHVControl * hvControl, bool viewOnly, QWidget *parent) :
	QWidget(parent)
{
	hvControl_ = hvControl;

	contentLayout_ = new QHBoxLayout;
	contentLayout_->setContentsMargins(1, 5, 1, 5);
	setLayout(contentLayout_);

	layoutHVControl(hvControl, viewOnly);
	setupConnections();
}


void CLSHVControlChannelView::onHVControlStatusChanged(int status)
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

void CLSHVControlChannelView::onHVControlMeasuredCurrentValueChanged(double value)
{
	if (measuredCurrentLabel_)
		measuredCurrentLabel_->setText(QString("%1 uA").arg(value));
}

void CLSHVControlChannelView::onHVControlVoltageValueChanged(double value)
{
	if (actVoltageLabel_)
		actVoltageLabel_->setText(QString("%1 V").arg(value));
	else if (voltageSpinBox_)
		voltageSpinBox_->setValue(value);
}

void CLSHVControlChannelView::onVoltageSpinBoxEditFinished()
{
	double voltage = voltageSpinBox_->value();
	hvControl_->move(voltage);
}

void CLSHVControlChannelView::layoutHVControl(CLSHVControl *hvControl, bool viewOnly)
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

	measuredCurrentLabel_ = 0;
	if (hvControl->hasMeasureCurrent()) {
		measuredCurrentLabel_ = new QLabel("0.00 uA");
		measuredCurrentLabel_->setFixedWidth(90);
		measuredCurrentLabel_->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	}

	powerOnOffButton_ = new QPushButton("Turn Off");
	powerOnOffButton_->setMaximumWidth(60);

	statusLabel_ = new QLabel;
	statusLabel_->setPixmap(QIcon(":/32x32/greenLEDOff.png").pixmap(22));

	contentLayout_->addWidget(nameLabel_, 0, Qt::AlignLeft);
	if (viewOnly)
		contentLayout_->addWidget(actVoltageLabel_);
	else
		contentLayout_->addWidget(voltageSpinBox_);

	if (measuredCurrentLabel_)
		contentLayout_->addWidget(measuredCurrentLabel_);

	contentLayout_->addWidget(powerOnOffButton_);
	contentLayout_->addWidget(statusLabel_);
}

void CLSHVControlChannelView::setupConnections()
{
	connect(hvControl_, SIGNAL(powerStatusChanged(int)), this, SLOT(onHVControlStatusChanged(int)));
	connect(hvControl_, SIGNAL(voltageValueChanged(double)), this, SLOT(onHVControlVoltageValueChanged(double)));
	if (measuredCurrentLabel_)
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
