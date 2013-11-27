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

    quickControls_ = new StripToolQuickControls(this);
    quickControls_->setModel(model_);

    QHBoxLayout *dataLayout = new QHBoxLayout();
    dataLayout->addWidget(plotView_);
    dataLayout->addWidget(quickControls_);

    QCheckBox *controlsToggle = new QCheckBox("Show controls", this);
    controlsToggle->setChecked(true);
    connect(controlsToggle, SIGNAL(stateChanged(int)), this, SLOT(toggleControls(int)) );

    QVBoxLayout *windowLayout = new QVBoxLayout();
    windowLayout->addLayout(dataLayout);
    windowLayout->addWidget(controlsToggle);

    setLayout(windowLayout);
}



void StripToolView::toggleControls(int checkState)
{
    if (checkState == 0)
        quickControls_->hide();
    else
        quickControls_->show();
}
