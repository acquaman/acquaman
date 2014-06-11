#include "STWidget.h"

#include <QDebug>

STWidget::STWidget(QWidget *parent)
    : QWidget(parent)
{
    dataCount_ = 0;
    displayCount_ = 10;

    ringCurrentControl_ = new AMReadOnlyPVControl("PCT1402-01:mA:fbk", "PCT1402-01:mA:fbk", this);
    connect( ringCurrentControl_, SIGNAL(valueChanged(double)), this, SLOT(onRingCurrentValueChanged(double)) );
    connect( ringCurrentControl_, SIGNAL(connected(bool)), this, SLOT(onRingCurrentConnected(bool)) );

    ringCurrentModel_ = new MPlotRealtimeModel();
    ringCurrentSeries_ = new MPlotSeriesBasic(ringCurrentModel_);

    plot_ = new MPlot();

    ringCurrentLabel_ = new QLabel("Storage ring current : ---");

    plotWidget_ = new MPlotWidget(this);
    plotWidget_->setPlot(plot_);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(ringCurrentLabel_);
    layout->addWidget(plotWidget_);

    setLayout(layout);
}

STWidget::~STWidget()
{

}

void STWidget::onRingCurrentValueChanged(double newValue)
{
    ringCurrentLabel_->setText(QString("Storage ring current : %1 mA").arg(newValue, 0, 'f', 3));

    ringCurrentModel_->insertPointBack(dataCount_, newValue);

    if (ringCurrentModel_->count() > displayCount_)
        ringCurrentModel_->removePointFront();

    dataCount_++;
}

void STWidget::onRingCurrentConnected(bool isConnected)
{
    if (isConnected)
        plot_->addItem(ringCurrentSeries_);
}
