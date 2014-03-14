#include "StripToolControlsPanel.h"
#include <QDebug>

StripToolControlsPanel::StripToolControlsPanel(QWidget *parent) : QWidget(parent)
{
    nameEntry_ = 0;
    pauseResume_ = 0;
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
    pauseResume_ = new DoubleButtonWidget(this);
    pauseResume_->setLeftButtonText("Pause");
    pauseResume_->setRightButtonText("Resume");
    timeEntry_ = new TimeEntryWidget(this);
    sidebarButton_ = new QPushButton(this);
    waterfallEntry_ = new WaterfallEntryWidget(this);
}



void StripToolControlsPanel::makeConnections()
{

//    connect( pauseResume_, SIGNAL(leftButtonClicked()), this, SIGNAL(pauseButtonClicked()) );
//    connect( pauseResume_, SIGNAL(rightButtonClicked()), this, SLOT(resumeButtonClicked()) );

//    connect( timeEntry_, SIGNAL(timeAmountChanged(int)), model_, SLOT(toUpdateTime(int)) );
//    connect( timeEntry_, SIGNAL(timeUnitsChanged(QString)), model_, SLOT(toUpdateTimeUnits(QString)) );
//    connect( model_, SIGNAL(requestTimeUpdate()), timeEntry_, SLOT(timeUpdateRequested()) );

//    connect( waterfallEntry_, SIGNAL(waterfallOn(bool)), model_, SIGNAL(waterfallStateChanged(bool)) );
//    connect( model_, SIGNAL(changeWaterfallCheckState(bool)), waterfallEntry_, SLOT(setWaterfallCheckState(bool)) );

}



void StripToolControlsPanel::defaultSettings()
{
    QGridLayout *panelLayout = new QGridLayout();
    panelLayout->addWidget(nameEntry_, 0, 0);
    panelLayout->addWidget(pauseResume_, 0, 1);
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



