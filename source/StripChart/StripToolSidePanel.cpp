#include "StripToolSidePanel.h"

StripToolSidePanel::StripToolSidePanel(QWidget *parent) :
    QWidget(parent)
{
    model_ = 0;

    listView_ = new StripToolListView(this);

    QVBoxLayout *controlsLayout = new QVBoxLayout();
    controlsLayout->addWidget(listView_);

    QGroupBox *controlsGroup = new QGroupBox();
    controlsGroup->setLayout(controlsLayout);

    QVBoxLayout *contentLayout = new QVBoxLayout();
    contentLayout->addWidget(controlsGroup);

    setLayout(contentLayout);
    setMaximumWidth(205);
}



StripToolSidePanel::~StripToolSidePanel()
{
}



void StripToolSidePanel::setModel(StripToolModel *newModel)
{
    model_ = newModel;

    connect( this, SIGNAL(pausePVs()), model_, SLOT(toPausePVs()) );
    connect( this, SIGNAL(resumePVs()), model_, SLOT(toResumePVs()) );

    listView_->setPVModel(model_);
}
