#include "STWidget.h"

#include <QDebug>

STWidget::STWidget(QWidget *parent) : QWidget(parent)
{
    dataCount_ = 0;
    displayCount_ = 10;

    ringCurrentControl_ = new AMProcessVariable("PCT1402-01:mA:fbk", true, this);
    connect( ringCurrentControl_, SIGNAL(valueChanged(double)), this, SLOT(onRingCurrentValueChanged(double)) );
    connect( ringCurrentControl_, SIGNAL(connected(bool)), this, SLOT(onRingCurrentConnected(bool)) );

    ringCurrentModel1_ = new MPlotRealtimeModel();

    accumulator_ = new AM0DAccumulatorAB("Ring current AB", this);
    accumulator_->setInputDataSources(QList<AMDataSource*>() << new AM0DProcessVariableDataSource(ringCurrentControl_, "SR1 Current", this));
    accumulator_->setMaximumStored(10);
    ringCurrentModel2_ = new AMDataSourceSeriesData(accumulator_);

    ringCurrentSeries1_ = new MPlotSeriesBasic();
    ringCurrentSeries1_->setModel(ringCurrentModel1_, true);
    ringCurrentSeries1_->setDescription("Series 1");

    ringCurrentSeries2_ = new MPlotSeriesBasic();
    ringCurrentSeries2_->setModel( ringCurrentModel2_, true);
    ringCurrentSeries2_->setDescription("Series 2");

    // create plot and set up axes.
    plot_ = new MPlot();
    plot_->axisBottom()->setAxisNameFont(QFont("Helvetica", 6));
    plot_->axisBottom()->setTickLabelFont(QFont("Helvetica", 6));
    plot_->axisBottom()->setAxisName("Counts");
    plot_->axisLeft()->setAxisNameFont(QFont("Helvetica", 6));
    plot_->axisLeft()->setTickLabelFont(QFont("Helvetica", 6));
    plot_->axisLeft()->setAxisName("Storage ring current [mA]");

    plot_->axisScaleLeft()->setAutoScaleEnabled();

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
    qDebug() << "PV update : " << newValue;

    ringCurrentLabel_->setText(QString("Storage ring current : %1 mA").arg(newValue, 0, 'f', 3));

    ringCurrentModel1_->insertPointBack(dataCount_, newValue);
    dataCount_++;

    if (dataCount_ > 10) {
        ringCurrentModel1_->removePointFront();
    }

    if (accumulator_->updatesStored() > 0) {
        qDebug() << "AB update : " << (double)accumulator_->value( AMnDIndex(accumulator_->updatesStored() - 1) );

        QVector<double> vals = QVector<double>(0);
        accumulator_->values(AMnDIndex(), AMnDIndex(accumulator_->updatesStored() - 1), vals.data());
        qDebug() << "Total values entries  :" << vals.size();
        qDebug() << "Total values : " << vals.toList();
    }
}

void STWidget::onRingCurrentConnected(bool isConnected)
{
    if (isConnected) {
        plot_->addItem(ringCurrentSeries1_);
        plot_->addItem(ringCurrentSeries2_);
    }
}
