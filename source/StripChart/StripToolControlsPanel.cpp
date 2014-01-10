#include "StripToolControlsPanel.h"

StripToolControlsPanel::StripToolControlsPanel(QWidget *parent) : QWidget(parent)
{
    nameEntry_ = new EntryWidget(this);

    pauseResume_ = new DoubleButtonWidget(this);
    pauseResume_->setLeftButtonText("Pause");
    pauseResume_->setRightButtonText("Resume");
    connect( pauseResume_, SIGNAL(leftButtonClicked()), this, SLOT(onPauseButtonClicked()) );
    connect( pauseResume_, SIGNAL(rightButtonClicked()), this, SLOT(onResumeButtonClicked()) );

    timeEntry_ = new TimeEntryWidget(this);

    sidebarShown_ = false;
    sidebarButton_ = new QPushButton(">");
    connect( sidebarButton_, SIGNAL(clicked()), this, SLOT(toToggleSidebar()) );

    waterfallEntry_ = new WaterfallEntryWidget();
//    waterfallEntry_->setEnabled(false);

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



StripToolControlsPanel::~StripToolControlsPanel()
{
}



void StripToolControlsPanel::setModel(StripToolModel *newModel)
{
    model_ = newModel;

    connect( nameEntry_, SIGNAL(entryComplete(QString)), model_, SLOT(toAddPV(QString)) );

    connect( pauseResume_, SIGNAL(leftButtonClicked()), model_, SLOT(toPausePVs()) );
    connect( pauseResume_, SIGNAL(rightButtonClicked()), model_, SLOT(toResumePVs()) );

    connect( timeEntry_, SIGNAL(timeAmountChanged(int)), model_, SLOT(toUpdateTime(int)) );
    connect( timeEntry_, SIGNAL(timeUnitsChanged(QString)), model_, SLOT(toUpdateTimeUnits(QString)) );
    connect( model_, SIGNAL(requestTimeUpdate()), timeEntry_, SLOT(timeUpdateRequested()) );

    connect( waterfallEntry_, SIGNAL(waterfallChanged(double)), model_, SIGNAL(waterfallChanged(double)) );
//    connect( model_, SIGNAL(selectedWaterfall(double)), waterfallEntry_, SLOT(toSetWaterfallDisplayed(double)) );
}



void StripToolControlsPanel::onPauseButtonClicked()
{
    pauseResume_->setRightButtonDefault(true);
}



void StripToolControlsPanel::onResumeButtonClicked()
{
    pauseResume_->setRightButtonDefault(false);
}



void StripToolControlsPanel::toToggleSidebar()
{
    if (sidebarShown_)
    {
        sidebarButton_->setText(">");
        sidebarShown_ = false;
        emit hideSidebar();

    } else {
        sidebarButton_->setText("<");
        sidebarShown_ = true;
        emit showSidebar();
    }
}



void StripToolControlsPanel::toTestSignal(const QString &signalText)
{
    qDebug() << "Controls panel received signal text :" << signalText;
}


