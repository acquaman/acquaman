#include "WaterfallEntryWidget.h"

WaterfallEntryWidget::WaterfallEntryWidget(QWidget *parent) :
    QWidget(parent)
{
    waterfallLabel_ = new QLabel("Percent offset : ");

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
    waterfallAmount_->setMinimum(-0.5);
    waterfallAmount_->setMaximum(0.5);
    waterfallAmount_->setSingleStep(0.1);

    waterfallAmount_->setValue(0.0);
}



void WaterfallEntryWidget::toSetWaterfallDisplayed(double newDisplay)
{
    waterfallAmount_->setValue(newDisplay);
}
