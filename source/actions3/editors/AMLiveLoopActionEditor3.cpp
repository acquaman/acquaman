#include "AMLiveLoopActionEditor3.h"

#include <QSpinBox>
#include <QHBoxLayout>
#include <QLabel>

AMLiveLoopActionEditor3::AMLiveLoopActionEditor3(AMLoopAction3 *action, QWidget *parent)
	: QFrame(parent)
{
	action_ = action;
	setFrameStyle(QFrame::StyledPanel);

	loopCount_ = new QSpinBox();
	loopCount_->setRange(action_->currentIteration(), 9999);
	loopCount_->setValue(action_->loopCount());

	iteration_ = new QLabel(QString("%1").arg(action_->currentIteration()));

	connect(action_, SIGNAL(currentIterationChanged(int)), this, SLOT(onIterationUpdate(int)));
	connect(loopCount_, SIGNAL(valueChanged(int)), this, SLOT(onSpinBoxValueChanged(int)));

	QHBoxLayout *hl = new QHBoxLayout(this);
	hl->addWidget(new QLabel("Current: "));
	hl->addWidget(iteration_);
	hl->addWidget(new QLabel(" of "));
	hl->addWidget(loopCount_);
	hl->addStretch();
}

void AMLiveLoopActionEditor3::onIterationUpdate(int val)
{
	loopCount_->setMinimum(val);
	iteration_->setText(QString::number(val));
}

void AMLiveLoopActionEditor3::onSpinBoxValueChanged(int val)
{
	action_->setLoopCount(val);
}
