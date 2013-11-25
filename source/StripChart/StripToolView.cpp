#include "StripChart/StripToolView.h"

StripToolView::StripToolView(QWidget *parent, StripToolModel *model) : QWidget(parent)
{
    model_ = model;
    buildUI();
}



StripToolView::~StripToolView()
{
}



void StripToolView::buildUI()
{
    plotView_ = new StripToolPlot(this);
    plotView_->setModel(model_);

    QCheckBox *controlsToggle = new QCheckBox("Show controls", this);
    controlsToggle->setChecked(true);

    QVBoxLayout *plotLayout = new QVBoxLayout();
    plotLayout->addWidget(plotView_);
    plotLayout->addWidget(controlsToggle);

    quickControls_ = new StripToolQuickControls(this);
    quickControls_->setModel(model_);
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
