#include "STWidget.h"

#include <QDebug>

STWidget::STWidget(QWidget *parent) : QWidget(parent)
{
    dataCount_ = 0;

    ringCurrentControl_ = new AMProcessVariable("PCT1402-01:mA:fbk", true, this);
    connect( ringCurrentControl_, SIGNAL(valueChanged(double)), this, SLOT(onRingCurrentValueChanged(double)) );

    data_ = new AM0DAccumulatorAB("RingCurrent", this);
    data_->setInputDataSources(QList<AMDataSource*>() << new AM0DProcessVariableDataSource(ringCurrentControl_, "SR1 Current", this));

    times_ = new AM0DTimestampAB("Timestamps", this);
    times_->setTimeUnits(AM0DTimestampAB::Seconds);
    times_->setInputDataSources(QList<AMDataSource*>() << new AM0DProcessVariableDataSource(ringCurrentControl_, "SR1 Current", this));

    timedData_ = new AM1DTimedDataAB("TimedData", this);
    timedData_->setInputDataSources(QList<AMDataSource*>() << data_ << times_);

    filteredData_ = new AMTimestampFilterAB("TimeFiltered", this);
    filteredData_->setTimeValue(10);
    filteredData_->setInputDataSources(QList<AMDataSource*>() << timedData_);

    ringCurrentModel2_ = new AMDataSourceSeriesData(filteredData_);

    ringCurrentSeries2_ = new MPlotSeriesBasic();
    ringCurrentSeries2_->setModel( ringCurrentModel2_, true);
    ringCurrentSeries2_->setDescription(" ");

    // create plot and set up axes.
    plot_ = new MPlot();
    plot_->axisBottom()->setAxisNameFont(QFont("Helvetica", 6));
    plot_->axisBottom()->setTickLabelFont(QFont("Helvetica", 6));
    plot_->axisBottom()->setAxisName("Time");
    plot_->axisLeft()->setAxisNameFont(QFont("Helvetica", 6));
    plot_->axisLeft()->setTickLabelFont(QFont("Helvetica", 6));
    plot_->axisLeft()->setAxisName("Storage ring current [mA]");

    // add series to plot.
    plot_->addItem(ringCurrentSeries2_);

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

void STWidget::onRingCurrentValueChanged(double newValue)
{
    qDebug() << "Ring current update.";

    ringCurrentLabel_->setText(QString("Storage ring current : %1 mA\n%2").arg(newValue, 0, 'f', 3).arg(dataCount_));
}
