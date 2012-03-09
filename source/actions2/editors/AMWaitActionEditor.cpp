#include "AMWaitActionEditor.h"

#include <QDoubleSpinBox>
#include <QBoxLayout>
#include <QLabel>

AMWaitActionEditor::AMWaitActionEditor(AMWaitActionInfo *info, QWidget *parent)
    : QFrame(parent) {

    info_ = info;
    setFrameStyle(QFrame::StyledPanel);

    spinBox_ = new QDoubleSpinBox();
    spinBox_->setDecimals(1);
    spinBox_->setRange(0, 3600*24*7);

    QHBoxLayout* hl = new QHBoxLayout(this);
    hl->addWidget(new QLabel("Waiting for"));
    hl->addWidget(spinBox_);
    hl->addWidget(new QLabel("seconds."));
    hl->addStretch();

    spinBox_->setValue(info->secondsToWait());
    connect(spinBox_, SIGNAL(editingFinished()), this, SLOT(onSpinBoxEditingFinished()));
}

void AMWaitActionEditor::onSpinBoxEditingFinished()
{
    info_->setSecondsToWait(spinBox_->value());
}
