#include "STWidget.h"

#include <QDebug>

STWidget::STWidget(QWidget *parent) : QWidget(parent)
{
    ringCurrent_ = new STVariable("PCT1402-01:mA:fbk", this);
    connect( ringCurrent_, SIGNAL(connected(bool)), this, SLOT(onRingCurrentConnected(bool)) );

    plotWidget_ = new STPlotWidget();
    plotWidget_->setPlotName("Storage ring current");
    plotWidget_->showPlotName(true);
    connect( plotWidget_, SIGNAL(showPlotEditor(bool)), this, SLOT(showPlotEditor(bool)) );

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(plotWidget_);

    setLayout(layout);
    setWindowTitle("Storage Ring Current");
}

STWidget::~STWidget()
{

}

void STWidget::onRingCurrentConnected(bool isConnected)
{
    if (isConnected) {
        // add series to plot.
        plotWidget_->plot()->addItem(ringCurrent_->series());
    }
}

void STWidget::showPlotEditor(bool show)
{
    if (show) {
        QDialog editor;
    }
}
