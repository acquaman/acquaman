#include "STWidget.h"

#include <QDebug>

STWidget::STWidget(QWidget *parent) : QWidget(parent)
{
    dataCount_ = 0;
    displayCount_ = 10;

    ringCurrentControl_ = new AMProcessVariable("PCT1402-01:mA:fbk", true, this);
    connect( ringCurrentControl_, SIGNAL(valueChanged(double)), this, SLOT(onRingCurrentValueChanged(double)) );
    connect( ringCurrentControl_, SIGNAL(connected(bool)), this, SLOT(onRingCurrentConnected(bool)) );

//    ringCurrentModel1_ = new MPlotRealtimeModel();

//    accumulator_ = new AM0DAccumulatorAB("Ring current AB", this);
//    accumulator_->setInputDataSources(QList<AMDataSource*>() << new AM0DProcessVariableDataSource(ringCurrentControl_, "SR1 Current", this));
//    timer_ = new AMTimestampAB("Timestamp data", this);
//    timer_->setTimeUnits(AMTimestampAB::Seconds);
//    timer_->setInputDataSources(QList<AMDataSource*>() << accumulator_);
//    ringCurrentModel2_ = new AMDataSourceSeriesData(timer_);

    timedDataSource_ = new AMTimestampAccumulatorAB("Ring current AB", this);
    timedDataSource_->setInputDataSources(QList<AMDataSource*>() << new AM0DProcessVariableDataSource(ringCurrentControl_, "SR1 Current", this));
    ringCurrentModel2_ = new AMDataSourceSeriesData(timedDataSource_);

//    ringCurrentSeries1_ = new MPlotSeriesBasic();
//    ringCurrentSeries1_->setModel(ringCurrentModel1_, true);
//    ringCurrentSeries1_->setDescription("Series 1");

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
    ringCurrentLabel_->setText(QString("Storage ring current : %1 mA").arg(newValue, 0, 'f', 3));
}

void STWidget::onRingCurrentConnected(bool isConnected)
{
    if (isConnected) {
//        plot_->addItem(ringCurrentSeries1_);
        plot_->addItem(ringCurrentSeries2_);
    }
}
