#include "StripToolControlsPanel.h"
#include <QDebug>

StripToolControlsPanel::StripToolControlsPanel(QWidget *parent) : QWidget(parent)
{
    nameEntry_ = 0;
//    pauseResume_ = 0;
    timeEntry_ = 0;
    sidebarButton_ = 0;
    waterfallEntry_ = 0;

    buildComponents();
    makeConnections();
    defaultSettings();

    qDebug() << "StripToolControlsPanel object created.";
}



StripToolControlsPanel::~StripToolControlsPanel()
{
}



EntryWidget* StripToolControlsPanel::nameEntry() const {
    return nameEntry_;
}



WaterfallEntryWidget* StripToolControlsPanel::waterfallEntry() const {
    return waterfallEntry_;
}



TimeEntryWidget* StripToolControlsPanel::timeEntry() const {
    return timeEntry_;
}



QPushButton* StripToolControlsPanel::sidebarButton() const {
    return sidebarButton_;
}



void StripToolControlsPanel::buildComponents()
{
    nameEntry_ = new EntryWidget(this);
    timeEntry_ = new TimeEntryWidget(this);
    sidebarButton_ = new QPushButton(this);
    waterfallEntry_ = new WaterfallEntryWidget(this);
}



void StripToolControlsPanel::makeConnections()
{

}



void StripToolControlsPanel::defaultSettings()
{
    QGridLayout *panelLayout = new QGridLayout();
    panelLayout->addWidget(nameEntry_, 0, 0);
//    panelLayout->addWidget(pauseResume_, 0, 1);
    panelLayout->addWidget(timeEntry_, 0, 2);
    panelLayout->addWidget(sidebarButton_, 0, 3);
    panelLayout->addWidget(waterfallEntry_, 1, 0);

    QGroupBox *controlsGroup = new QGroupBox();
    controlsGroup->setLayout(panelLayout);
    controlsGroup->setFlat(true);

    QVBoxLayout *widgetLayout = new QVBoxLayout();
    widgetLayout->addWidget(controlsGroup);

    setLayout(widgetLayout);
}



