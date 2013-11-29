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
    quickControls_->hide();

    controlPanel_ = new StripToolControlsPanel(this);
    controlPanel_->setModel(model_);
    connect( controlPanel_, SIGNAL(showSidebar()), quickControls_, SLOT(show()) );
    connect( controlPanel_, SIGNAL(hideSidebar()), quickControls_, SLOT(hide()) );

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(plotView_);
    mainLayout->addWidget(controlPanel_);

    QHBoxLayout *windowLayout = new QHBoxLayout();
    windowLayout->addLayout(mainLayout);
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
