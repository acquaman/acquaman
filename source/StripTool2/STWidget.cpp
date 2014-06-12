#include "STWidget.h"

#include <QDebug>

STWidget::STWidget(QWidget *parent)
    : QWidget(parent)
{
    dataCount_ = 0;
    displayCount_ = 10;

    ringCurrentControl_ = new AMProcessVariable("PCT1402-01:mA:fbk", true, this);
    connect( ringCurrentControl_, SIGNAL(valueChanged(double)), this, SLOT(onRingCurrentValueChanged(double)) );
    connect( ringCurrentControl_, SIGNAL(connected(bool)), this, SLOT(onRingCurrentConnected(bool)) );

    ringCurrentModel1_ = new MPlotRealtimeModel();
//    ringCurrentModel2_ = new AMDataSourceSeriesData( new, this);

    ringCurrentSeries1_ = new MPlotSeriesBasic();
    ringCurrentSeries1_->setModel(ringCurrentModel1_, true);
    ringCurrentSeries1_->setDescription("Series 1");

    ringCurrentSeries2_ = new MPlotSeriesBasic();
//    ringCurrentSeries2_->setModel(ringCurrentModel1_, false);
//    ringCurrentSeries2_->setDescription("Series 2");

//    summedData_ = new AM1DSummingAB("Summed Current", this);
//    summedData_->setInputDataSources(QList<AMDataSource*>() << ringCurrentModel1_ << ringCurrentModel2_);

    ringCurrentSeries3_ = new MPlotSeriesBasic();
//    ringCurrentSeries3_->setModel(new AMDataSourceSeriesData(summedData_), true);

    // create plot and set up axes.
    plot_ = new MPlot();
    plot_->axisBottom()->setAxisNameFont(QFont("Helvetica", 6));
    plot_->axisBottom()->setTickLabelFont(QFont("Helvetica", 6));
    plot_->axisBottom()->setAxisName("X");
    plot_->axisLeft()->setAxisNameFont(QFont("Helvetica", 6));
    plot_->axisLeft()->setTickLabelFont(QFont("Helvetica", 6));
    plot_->axisLeft()->setAxisName("Y");

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

    ringCurrentModel1_->insertPointBack(dataCount_, newValue);

    if (ringCurrentModel1_->count() > displayCount_)
        ringCurrentModel1_->removePointFront();

    dataCount_++;
}

void STWidget::onRingCurrentConnected(bool isConnected)
{
    if (isConnected) {
        plot_->addItem(ringCurrentSeries1_);
//        plot_->addItem(ringCurrentSeries2_);
//        plot_->addItem(ringCurrentSeries3_);
    }
}
