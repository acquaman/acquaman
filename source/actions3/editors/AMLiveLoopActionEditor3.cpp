#include "AMLiveLoopActionEditor3.h"

#include <QSpinBox>
#include <QHBoxLayout>
#include <QLabel>

 AMLiveLoopActionEditor3::~AMLiveLoopActionEditor3(){}
AMLiveLoopActionEditor3::AMLiveLoopActionEditor3(AMLoopAction3 *action, QWidget *parent)
	: QFrame(parent)
{
	action_ = action;
	setFrameStyle(QFrame::StyledPanel);

	loopCount_ = new QSpinBox();
	loopCount_->setRange(action_->currentIteration()+1, 9999);
	loopCount_->setValue(action_->loopCount());

	iteration_ = new QLabel(QString("%1").arg(action_->currentIteration()+1));

	connect(action_, SIGNAL(currentIterationChanged(int)), this, SLOT(onIterationUpdate(int)));
	connect(action_->info(), SIGNAL(infoChanged()), this, SLOT(onLoopCountChanged()));
	connect(loopCount_, SIGNAL(editingFinished()), this, SLOT(setLoopCount()));

	QHBoxLayout *hl = new QHBoxLayout(this);
	hl->addWidget(new QLabel("Current: "));
	hl->addWidget(iteration_);
	hl->addWidget(new QLabel(" of "));
	hl->addWidget(loopCount_);
	hl->addStretch();
}

void AMLiveLoopActionEditor3::onIterationUpdate(int val)
{
	loopCount_->setMinimum(val+1);
	iteration_->setText(QString::number(val+1));
}

void AMLiveLoopActionEditor3::onLoopCountChanged()
{
	loopCount_->blockSignals(true);
	loopCount_->setValue(action_->loopCount());
	loopCount_->blockSignals(false);
}

void AMLiveLoopActionEditor3::setLoopCount()
{
	action_->setLoopCount(loopCount_->value());
}
