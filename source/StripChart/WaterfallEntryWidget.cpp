#include "WaterfallEntryWidget.h"

WaterfallEntryWidget::WaterfallEntryWidget(QWidget *parent) :
    QWidget(parent)
{
    waterfallCheckBox_ = new QCheckBox("Waterfall on");
    connect( waterfallCheckBox_, SIGNAL(toggled(bool)), this, SIGNAL(waterfallOn(bool)) );

    QHBoxLayout *waterfallLayout = new QHBoxLayout();
    waterfallLayout->addWidget(waterfallCheckBox_);

    setLayout(waterfallLayout);
}



void WaterfallEntryWidget::setWaterfallCheckState(bool on)
{
    if (on)
        waterfallCheckBox_->setCheckState(Qt::Checked);
    else
        waterfallCheckBox_->setCheckState(Qt::Unchecked);
}
