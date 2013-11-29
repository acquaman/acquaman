#include "StripToolQuickControls.h"

StripToolQuickControls::StripToolQuickControls(QWidget *parent) :
    QWidget(parent)
{
    model_ = 0;

    listView_ = new StripToolListView(this);

    pauseButton_ = new QPushButton("Pause");
    connect( pauseButton_, SIGNAL(clicked()), this, SLOT(pauseClicked()) );

    resumeButton_ = new QPushButton("Resume");
    connect( resumeButton_, SIGNAL(clicked()), this, SLOT(resumeClicked()) );

    QHBoxLayout *pauseResumeLayout = new QHBoxLayout();
    pauseResumeLayout->addWidget(pauseButton_);
    pauseResumeLayout->addWidget(resumeButton_);

    QGroupBox *pauseResumeGroup = new QGroupBox();
    pauseResumeGroup->setLayout(pauseResumeLayout);
    pauseResumeGroup->setFlat(true);

    QLabel *timeLabel = new QLabel("Time :");

    QSpinBox *timeEntry = new QSpinBox();
    timeEntry->setValue(10);
    connect( timeEntry, SIGNAL(valueChanged(int)), this, SLOT(timeValueChanged(int)) );

    timeUnits_ = new QComboBox();
    timeUnits_->addItem("seconds");
    timeUnits_->addItem("minutes");
    timeUnits_->addItem("hours");
    timeUnits_->setEditable(false);
    connect( timeUnits_, SIGNAL(currentIndexChanged(QString)), this, SLOT(timeUnitsSelected(QString)) );

    QHBoxLayout *timeLayout = new QHBoxLayout();
    timeLayout->addWidget(timeLabel);
    timeLayout->addWidget(timeEntry);
    timeLayout->addWidget(timeUnits_);

    QGroupBox *timeGroup = new QGroupBox();
    timeGroup->setLayout(timeLayout);
    timeGroup->setFlat(true);

    message_ = new QLabel("");
    connect( this, SIGNAL(clearMessage()), message_, SLOT(clear()) );

    QVBoxLayout *controlsLayout = new QVBoxLayout();
    controlsLayout->addWidget(listView_);
    controlsLayout->addWidget(pauseResumeGroup);
    controlsLayout->addWidget(timeGroup);

    QGroupBox *controlsGroup = new QGroupBox();
    controlsGroup->setLayout(controlsLayout);

    QVBoxLayout *contentLayout = new QVBoxLayout();
    contentLayout->addWidget(controlsGroup);

    setLayout(contentLayout);
    setMaximumWidth(225);

    connect( this, SIGNAL(reset()), this, SLOT(resetControls()) );
}



StripToolQuickControls::~StripToolQuickControls() {
}



void StripToolQuickControls::setModel(StripToolModel *newModel)
{
    model_ = newModel;

    connect( this, SIGNAL(addPV(QString)), model_, SLOT(toAddPV(QString)) );
    connect( this, SIGNAL(pausePVs()), model_, SLOT(toPausePVs()) );
    connect( this, SIGNAL(resumePVs()), model_, SLOT(toResumePVs()) );
    connect( this, SIGNAL(updateTime(int)), model_, SLOT(toUpdateTime(int)) );

    connect( model_, SIGNAL(pvValid(bool)), this, SLOT(resetControls()) );

    listView_->setPVModel(model_);
}



void StripToolQuickControls::resetControls()
{
    emit clearName();
    emit buttonEnabled(true);
    emit nameEntryEnabled(true);
    emit nameEntryFocus();
}



//void StripToolQuickControls::displayMessage(const QString &text)
//{
//    message_->setText(text);
//    emit reset();
//}



//void StripToolQuickControls::addClicked()
//{
//    emit buttonEnabled(false);
//    emit nameEntryEnabled(false);

//    QString pvName = pvNameLineEdit_->text();

//    if (pvName == "")
//    {
//        emit error("PV name cannot be blank.");

//    } else {

//        emit addPV(pvName);
//    }
//}



void StripToolQuickControls::pauseClicked()
{
    emit pausePVs();
    resumeButton_->setDefault(true);
}



void StripToolQuickControls::resumeClicked()
{
    emit resumePVs();
    resumeButton_->setDefault(false);
}



void StripToolQuickControls::timeValueChanged(int newTime)
{
    emit updateTime(newTime);
}



QString StripToolQuickControls::getCurrentUnits() const
{
    return timeUnits_->currentText();
}



void StripToolQuickControls::timeUnitsSelected(const QString &newUnits)
{
    qDebug() << "New units :" << newUnits;
    emit updateUnits(newUnits);
}
