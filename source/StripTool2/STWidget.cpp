#include "STWidget.h"

#include <QDebug>

STWidget::STWidget(QWidget *parent) : QWidget(parent)
{
    ringCurrentControl_ = new AMProcessVariable("PCT1402-01:mA:fbk", true, this);
    connect( ringCurrentControl_, SIGNAL(valueChanged(double)), this, SLOT(onRingCurrentValueChanged(double)) );
    connect( ringCurrentControl_, SIGNAL(connected(bool)), this, SLOT(onRingCurrentConnected(bool)) );

    data_ = new AM0DAccumulatorAB("RingCurrent", this);
    data_->setDataStoredCountMax(50);
    data_->setInputDataSources(QList<AMDataSource*>() << new AM0DProcessVariableDataSource(ringCurrentControl_, "SR1 Current", this));

    times_ = new AM0DTimestampAB("Timestamps", this);
    times_->setDataStoredCountMax(50);
    times_->setTimeUnits(AM0DTimestampAB::Seconds);
    times_->setTimeValue(10);
    times_->enableTimeFiltering(true);
    times_->setInputDataSources(QList<AMDataSource*>() << new AM0DProcessVariableDataSource(ringCurrentControl_, "SR1 Current", this));

    timedData_ = new AM1DTimedDataAB("TimedData", this);
    timedData_->setInputDataSources(QList<AMDataSource*>() << data_ << times_);

    ringCurrentModel2_ = new AMDataSourceSeriesData(timedData_);

    ringCurrentSeries2_ = new MPlotSeriesBasic();
    ringCurrentSeries2_->setModel( ringCurrentModel2_, true);
    ringCurrentSeries2_->setDescription(" ");

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
        plot_->addItem(ringCurrentSeries2_);
    }
}

void STWidget::onRingCurrentValueChanged(double newValue)
{
    ringCurrentLabel_->setText(QString("Storage ring current : %1 mA").arg(newValue, 0, 'f', 3));
}
