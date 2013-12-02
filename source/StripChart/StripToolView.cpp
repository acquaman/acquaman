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

    sidePanel_ = new StripToolSidePanel(this);
    sidePanel_->setModel(model_);
    sidePanel_->hide();

    QHBoxLayout *upperLayout = new QHBoxLayout();
    upperLayout->addWidget(plotView_);
    upperLayout->addWidget(sidePanel_);

    controlPanel_ = new StripToolControlsPanel(this);
    controlPanel_->setModel(model_);
    connect( controlPanel_, SIGNAL(showSidebar()), sidePanel_, SLOT(show()) );
    connect( controlPanel_, SIGNAL(hideSidebar()), sidePanel_, SLOT(hide()) );

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(upperLayout);
    mainLayout->addWidget(controlPanel_);

    setLayout(mainLayout);
}
