#include "STWidget.h"

#include <QDebug>

STWidget::STWidget(QWidget *parent) : QWidget(parent)
{
    ringCurrentControl_ = new AMProcessVariable("PCT1402-01:mA:fbk", true, this);
    connect( ringCurrentControl_, SIGNAL(valueChanged(double)), this, SLOT(onRingCurrentValueChanged(double)) );

    ringCurrent_ = new STVariable("PCT1402-01:mA:fbk", this);
    connect( ringCurrent_, SIGNAL(connected(bool)), this, SLOT(onRingCurrentConnected(bool)) );


    // create plot and set up axes.
    plot_ = new MPlot();
    plot_->axisBottom()->setAxisName("Time");
    plot_->axisLeft()->setAxisName("Storage ring current [mA]");

    // create plot window
    plotWidget_ = new MPlotWidget(this);
    plotWidget_->enableAntiAliasing(true);
    plotWidget_->setPlot(plot_);

    ringCurrentLabel_ = new QLabel("Storage ring current : ---");

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(ringCurrentLabel_);
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
        plot_->addItem(ringCurrent_->series());
    }
}

void STWidget::onRingCurrentValueChanged(double newValue)
{
    ringCurrentLabel_->setText(QString("Storage ring current : %1 mA").arg(newValue, 0, 'f', 3));
}
