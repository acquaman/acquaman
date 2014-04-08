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



StripToolListView* StripToolSidePanel::listView() const
{
    return listView_;
}



EntryWidget* StripToolSidePanel::nameEntry() const
{
    return nameEntry_;
}



TimeEntryWidget* StripToolSidePanel::timeEntry() const
{
    return timeEntry_;
}



WaterfallEntryWidget* StripToolSidePanel::waterfallEntry() const
{
    return waterfallEntry_;
}



void StripToolSidePanel::buildComponents()
{
    listView_ = new StripToolListView(this);
    nameEntry_ = new EntryWidget(this);
    timeEntry_ = new TimeEntryWidget(this);
    waterfallEntry_ = new WaterfallEntryWidget(this);
}



void StripToolSidePanel::makeConnections()
{
}



void StripToolSidePanel::defaultSettings()
{
    QVBoxLayout *controlsLayout = new QVBoxLayout();
    controlsLayout->addWidget(listView_);
    controlsLayout->addWidget(nameEntry_);
    controlsLayout->addWidget(timeEntry_);
    controlsLayout->addWidget(waterfallEntry_);

    QGroupBox *controlsGroup = new QGroupBox();
    controlsGroup->setLayout(controlsLayout);

    QVBoxLayout *contentLayout = new QVBoxLayout();
    contentLayout->addWidget(controlsGroup);

    setLayout(contentLayout);
    setMaximumWidth(250);
}

