#include "StripChart/StripToolView.h"

StripToolView::StripToolView(QWidget *parent, StripToolModel *model) : QWidget(parent)
{
    model_ = model;

    createActions();
    buildUI();
}



StripToolView::~StripToolView()
{
}



void StripToolView::createActions()
{
    newPlotAction_ = new QAction("New plot", this);
    newPlotAction_->setEnabled(false);

    saveDataAction_ = new QAction("Save data", this);
    saveDataAction_->setEnabled(false);

    quitAction_ = new QAction("Quit", this);
    connect( quitAction_, SIGNAL(triggered()), qApp, SLOT(quit()) );
}



void StripToolView::buildUI()
{
    plotView_ = new StripToolPlot(this);
    connect( model_, SIGNAL(seriesChanged(Qt::CheckState, MPlotItem*)), plotView_, SLOT(onSeriesChanged(Qt::CheckState, MPlotItem*)) );
    connect( plotView_, SIGNAL(seriesSelected(MPlotItem*,bool)), model_, SLOT(seriesSelected(MPlotItem*, bool)) );
    connect( model_, SIGNAL(setPlotAxesLabels(QString,QString)), plotView_, SLOT(showPlotAxesLabels(QString, QString)) );

    QCheckBox *controlsToggle = new QCheckBox("Show controls", this);

    QCheckBox *legendToggle = new QCheckBox("Show legend", this);
    legendToggle->setEnabled(false);

    QVBoxLayout *plotLayout = new QVBoxLayout();
    plotLayout->addWidget(plotView_);
    plotLayout->addWidget(controlsToggle);
    plotLayout->addWidget(legendToggle);

    quickControls_ = new StripToolQuickControls(this, model_);
    quickControls_->hide();
    connect(controlsToggle, SIGNAL(stateChanged(int)), this, SLOT(toggleControls(int)) );

    QHBoxLayout *windowLayout = new QHBoxLayout();
    windowLayout->addLayout(plotLayout);
    windowLayout->addWidget(quickControls_);

    setLayout(windowLayout);
}



void StripToolView::toggleControls(int checkState)
{
    if (checkState == 0)
        quickControls_->hide();
    else
        quickControls_->show();
}



void StripToolView::toggleLegend(int checkState)
{
    Q_UNUSED(checkState);
}
