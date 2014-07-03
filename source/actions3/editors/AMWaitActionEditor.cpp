#include "AMWaitActionEditor.h"

#include <QHBoxLayout>
#include <QLabel>

AMWaitActionEditor::AMWaitActionEditor(AMWaitActionInfo *info, QWidget *parent)
	: QFrame(parent)
{
	info_ = info;
	setFrameStyle(QFrame::StyledPanel);

	time_ = new QDoubleSpinBox;
	time_->setRange(0, 100000);
	time_->setValue(info_->waitTime());
	connect(time_, SIGNAL(editingFinished()), this, SLOT(setWaitTime()));
	connect(info_, SIGNAL(waitTimeChanged(double)), this, SLOT(onWaitTimeChanged(double)));

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(new QLabel("Wait time:"));
	layout->addWidget(time_);
	layout->addStretch();

	setLayout(layout);
}

void AMWaitActionEditor::onWaitTimeChanged(double newTime)
{
	time_->blockSignals(true);
	time_->setValue(newTime);
	time_->blockSignals(false);
}

void AMWaitActionEditor::setWaitTime()
{
	info_->setWaitTime(time_->value());
}
