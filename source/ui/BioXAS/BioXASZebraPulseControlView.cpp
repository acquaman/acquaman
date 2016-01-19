#include "BioXASZebraPulseControlView.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>

BioXASZebraPulseControlView::BioXASZebraPulseControlView(BioXASZebraPulseControl *pulseControl, QWidget *parent)
	: QWidget(parent)
{
	pulseControl_ = pulseControl;

	inputSpinBox_ = new QSpinBox;
	inputSpinBox_->setRange(0, 63);
	inputSpinBox_->setValue(pulseControl_->inputValue());
	connect(inputSpinBox_, SIGNAL(editingFinished()), this, SLOT(setInputValue()));
	connect(pulseControl_, SIGNAL(inputValueChanged(int)), inputSpinBox_, SLOT(setValue(int)));

	inputLabel_ = new QLabel(pulseControl_->inputValueString());
	connect(pulseControl_, SIGNAL(inputValueStringChanged(QString)), inputLabel_, SLOT(setText(QString)));

	inputStatusLight_ = new QLabel;
	connect(pulseControl_, SIGNAL(inputValueStatusChanged(bool)), this, SLOT(onInputStatusChanged(bool)));

	risingEdgeButton_ = new QRadioButton("Rising");
	fallingEdgeButton_ = new QRadioButton("Falling");

	if (pulseControl_->edgeTriggerValue() == 1)
		fallingEdgeButton_->setChecked(true);
	else
		risingEdgeButton_->setChecked(true);

	connect(risingEdgeButton_, SIGNAL(toggled(bool)), this, SLOT(onEdgeTriggerButtonsChanged()));
	connect(pulseControl_, SIGNAL(edgeTriggerValueChanged(int)), this, SLOT(onEdgeTriggerValueChanged(int)));

	delayBeforeSpinBox_ = new QDoubleSpinBox;
	delayBeforeSpinBox_->setMinimum(0);
	delayBeforeSpinBox_->setValue(pulseControl_->delayTime());
	delayBeforeSpinBox_->setSuffix(" s");
	connect(delayBeforeSpinBox_, SIGNAL(editingFinished()), this, SLOT(setDelayBeforeValue()));
	connect(pulseControl_, SIGNAL(delayBeforeValueChanged(double)), this, SLOT(onDelayBeforeValueChanged()));

	pulseWidthSpinBox_ = new QDoubleSpinBox;
	pulseWidthSpinBox_->setRange(0, 65);
	pulseWidthSpinBox_->setValue(pulseControl_->pulseTime());
	pulseWidthSpinBox_->setSuffix(" s");
	connect(pulseWidthSpinBox_, SIGNAL(editingFinished()), this, SLOT(setPulseWidthValue()));
	connect(pulseControl_, SIGNAL(pulseWidthValueChanged(double)), this, SLOT(onPulseWidthValueChanged()));

	timeUnitsGroup_ = new QButtonGroup;
	QPushButton *msTimeUnitsButton = new QPushButton("ms");
	msTimeUnitsButton->setCheckable(true);
	timeUnitsGroup_->addButton(msTimeUnitsButton, 0);
	QPushButton *sTimeUnitsButton = new QPushButton("1s");
	sTimeUnitsButton->setCheckable(true);
	timeUnitsGroup_->addButton(sTimeUnitsButton, 1);
	QPushButton *tenSTimeUnitsButton = new QPushButton("10s");
	tenSTimeUnitsButton->setCheckable(true);
	timeUnitsGroup_->addButton(tenSTimeUnitsButton, 2);
	connect(timeUnitsGroup_, SIGNAL(buttonClicked(int)), pulseControl_, SLOT(setTimeUnitsValue(int)));
	connect(pulseControl_, SIGNAL(timeUnitsValueChanged(int)), this, SLOT(onTimeUnitsValueChanged(int)));
	onTimeUnitsValueChanged(pulseControl_->timeUnitsValue());

	triggerWhileActiveLight_ = new QLabel;
	connect(pulseControl_, SIGNAL(triggerWhileActiveValueChanged(bool)), this, SLOT(onTriggerWhileActiveChanged(bool)));
	onTriggerWhileActiveChanged(pulseControl_->triggerWhileActiveValue());

	outputStatusLight_ = new QLabel;
	connect(pulseControl_, SIGNAL(outputValueChanged(bool)), this, SLOT(onOutputStatusChanged(bool)));
	onOutputStatusChanged(pulseControl_->outputValue());

	QHBoxLayout *inputLayout = new QHBoxLayout;
	inputLayout->addWidget(inputSpinBox_);
	inputLayout->addWidget(inputLabel_);
	inputLayout->addWidget(inputStatusLight_);

	QHBoxLayout *triggerLayout = new QHBoxLayout;
	triggerLayout->addWidget(new QLabel("Trigger on edge"));
	triggerLayout->addWidget(risingEdgeButton_);
	triggerLayout->addWidget(fallingEdgeButton_);

	QHBoxLayout *delayBeforeLayout = new QHBoxLayout;
	delayBeforeLayout->addWidget(new QLabel("Delay before"));
	delayBeforeLayout->addWidget(delayBeforeSpinBox_);

	QHBoxLayout *pulseWidthLayout = new QHBoxLayout;
	pulseWidthLayout->addWidget(new QLabel("Pulse width"));
	pulseWidthLayout->addWidget(pulseWidthSpinBox_);

	QHBoxLayout *timeUnitsLayout = new QHBoxLayout;
	timeUnitsLayout->addWidget(new QLabel("Time Units"));
	timeUnitsLayout->addWidget(msTimeUnitsButton);
	timeUnitsLayout->addWidget(sTimeUnitsButton);
	timeUnitsLayout->addWidget(tenSTimeUnitsButton);

	QHBoxLayout *statusLayout = new QHBoxLayout;
	statusLayout->addWidget(triggerWhileActiveLight_);
	statusLayout->addWidget(new QLabel("Trigger while Active"));
	statusLayout->addWidget(outputStatusLight_);
	statusLayout->addWidget(new QLabel("Output Pulse"));

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(inputLayout);
	mainLayout->addLayout(triggerLayout);
	mainLayout->addLayout(delayBeforeLayout);
	mainLayout->addLayout(pulseWidthLayout);
	mainLayout->addLayout(timeUnitsLayout);
	mainLayout->addLayout(statusLayout);

	QGroupBox *groupBox = new QGroupBox(pulseControl_->name());
	groupBox->setLayout(mainLayout);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(groupBox);

	setLayout(layout);
}

void BioXASZebraPulseControlView::setInputValue()
{
	if (inputSpinBox_->value() != pulseControl_->inputValue())
		pulseControl_->setInputValue(inputSpinBox_->value());
}

void BioXASZebraPulseControlView::setDelayBeforeValue()
{
	if (delayBeforeSpinBox_->value() != pulseControl_->delayTime())
		pulseControl_->setDelayTime(delayBeforeSpinBox_->value());
}

void BioXASZebraPulseControlView::setPulseWidthValue()
{
	if (pulseWidthSpinBox_->value() != pulseControl_->pulseTime())
		pulseControl_->setPulseTime(pulseWidthSpinBox_->value());
}

void BioXASZebraPulseControlView::onInputStatusChanged(bool status)
{
	inputStatusLight_->setPixmap(QIcon(status ? ":/22x22/greenLEDOn.png" : ":/22x22/greenLEDOff.png").pixmap(22));
}

void BioXASZebraPulseControlView::onEdgeTriggerValueChanged(int value)
{
	if (value == 1)
		fallingEdgeButton_->setChecked(true);

	else
		risingEdgeButton_->setChecked(true);
}

void BioXASZebraPulseControlView::onEdgeTriggerButtonsChanged()
{
	pulseControl_->setEdgeTriggerValue(fallingEdgeButton_->isChecked() ? 1 : 0);
}

void BioXASZebraPulseControlView::onDelayBeforeValueChanged()
{
	delayBeforeSpinBox_->setValue(pulseControl_->delayTime());
}

void BioXASZebraPulseControlView::onPulseWidthValueChanged()
{
	pulseWidthSpinBox_->setValue(pulseControl_->pulseTime());
}

void BioXASZebraPulseControlView::onTimeUnitsValueChanged(int value)
{
	if (value != -1 && !timeUnitsGroup_->button(value)->isChecked())
		timeUnitsGroup_->button(value)->setChecked(true);

	onDelayBeforeValueChanged();
	onPulseWidthValueChanged();
}

void BioXASZebraPulseControlView::onTriggerWhileActiveChanged(bool status)
{
	triggerWhileActiveLight_->setPixmap(QIcon(status ? ":/22x22/redLEDOn.png" : ":/22x22/redLEDOff.png").pixmap(22));
}

void BioXASZebraPulseControlView::onOutputStatusChanged(bool status)
{
	outputStatusLight_->setPixmap(QIcon(status ? ":/22x22/greenLEDOn.png" : ":/22x22/greenLEDOff.png").pixmap(22));
}

