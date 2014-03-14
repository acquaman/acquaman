#include "StripToolSidePanel.h"

StripToolSidePanel::StripToolSidePanel(QWidget *parent) : QWidget(parent)
{
    listView_ = 0;

    buildComponents();
    makeConnections();
    defaultSettings();

    qDebug() << "StripToolSidePanel object created.";
}



StripToolSidePanel::~StripToolSidePanel()
{
}



StripToolListView* StripToolSidePanel::listView() const {
    return listView_;
}



void StripToolSidePanel::buildComponents()
{
    listView_ = new StripToolListView(this);
}



void StripToolSidePanel::makeConnections()
{
}



void StripToolSidePanel::defaultSettings()
{
    QVBoxLayout *controlsLayout = new QVBoxLayout();
    controlsLayout->addWidget(listView_);

    QGroupBox *controlsGroup = new QGroupBox();
    controlsGroup->setLayout(controlsLayout);

    QVBoxLayout *contentLayout = new QVBoxLayout();
    contentLayout->addWidget(controlsGroup);

    setLayout(contentLayout);
    setMaximumWidth(205);
}

