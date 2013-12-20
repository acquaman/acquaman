#include "WaterfallEntryWidget.h"

WaterfallEntryWidget::WaterfallEntryWidget(QWidget *parent) :
    QWidget(parent)
{
    waterfallLabel_ = new QLabel("Waterfall amount : ");

    waterfallAmount_ = new QDoubleSpinBox();
    connect( waterfallAmount_, SIGNAL(valueChanged(double)), this, SIGNAL(waterfallChanged(double)) );

    QHBoxLayout *waterfallLayout = new QHBoxLayout();
    waterfallLayout->addWidget(waterfallLabel_);
    waterfallLayout->addWidget(waterfallAmount_);

    setLayout(waterfallLayout);

    initialize();
}



void WaterfallEntryWidget::initialize()
{
    waterfallAmount_->setValue(0.0);
    waterfallAmount_->setMinimum(0.0);
    waterfallAmount_->setMaximum(1.0);
    waterfallAmount_->setSingleStep(0.1);
}
