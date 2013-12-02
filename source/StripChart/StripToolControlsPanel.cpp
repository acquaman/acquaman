#include "StripToolControlsPanel.h"

StripToolControlsPanel::StripToolControlsPanel(QWidget *parent) : QWidget(parent)
{
    nameEntry_ = new EntryWidget(this);
    nameEntry_->setMinimumWidth(200);
    connect( nameEntry_, SIGNAL(entryComplete(QString)), this, SLOT(toTestSignal(QString)) );

    QFrame *separator1 = new QFrame();
    separator1->setFrameShape(QFrame::VLine);
    separator1->setFrameShadow(QFrame::Sunken);
    separator1->setMaximumHeight(20);

    pauseResume_ = new DoubleButtonWidget(this);
    pauseResume_->setLeftButtonText("Pause");
    pauseResume_->setRightButtonText("Resume");
    pauseResume_->setMinimumWidth(200);
    connect( pauseResume_, SIGNAL(leftButtonClicked()), this, SLOT(onPauseButtonClicked()) );
    connect( pauseResume_, SIGNAL(rightButtonClicked()), this, SLOT(onResumeButtonClicked()) );

    QFrame *separator2 = new QFrame();
    separator2->setFrameShape(QFrame::VLine);
    separator2->setFrameShadow(QFrame::Sunken);
    separator2->setMaximumHeight(20);

    timeEntry_ = new TimeEntryWidget(this);
    timeEntry_->setMinimumWidth(200);

    QFrame *separator3 = new QFrame();
    separator3->setFrameShape(QFrame::VLine);
    separator3->setFrameShadow(QFrame::Sunken);
    separator3->setMaximumHeight(20);

    sidebarShown_ = false;
    sidebarButton_ = new QPushButton(">");
    connect( sidebarButton_, SIGNAL(clicked()), this, SLOT(toToggleSidebar()) );


    QHBoxLayout *controlsLayout = new QHBoxLayout();
    controlsLayout->addWidget(nameEntry_);
    controlsLayout->addWidget(separator1);
    controlsLayout->addWidget(pauseResume_);
    controlsLayout->addWidget(separator2);
    controlsLayout->addWidget(timeEntry_);
    controlsLayout->addWidget(separator3);
    controlsLayout->addWidget(sidebarButton_);

    setLayout(controlsLayout);

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

    connect( timeEntry_, SIGNAL(timeUnitsChanged(QString)), model_, SLOT(toTestSignal(QString)) );
    connect( timeEntry_, SIGNAL(timeUnitsChanged(QString)), this, SLOT(toTestSignal(QString)) );

    timeEntry_->initialize();
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


