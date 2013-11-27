#include "StripToolQuickControls.h"

StripToolQuickControls::StripToolQuickControls(QWidget *parent) :
    QWidget(parent)
{
    model_ = 0;

    listView_ = new StripToolListView(this);

    pvNameLineEdit_ = new QLineEdit();
    pvNameLineEdit_->setFocus(Qt::OtherFocusReason);
    connect( this, SIGNAL(nameEntryEnabled(bool)), pvNameLineEdit_, SLOT(setEnabled(bool)) );
    connect( this, SIGNAL(clearName()), pvNameLineEdit_, SLOT(clear()) );
    connect( pvNameLineEdit_, SIGNAL(returnPressed()), this, SLOT(addClicked()) );
    connect( pvNameLineEdit_, SIGNAL(textEdited(QString)), this, SIGNAL(clearMessage()) );

    addButton_ = new QPushButton("Add");
    connect( addButton_, SIGNAL(clicked()), this, SLOT(addClicked()) );
    connect( this, SIGNAL(buttonEnabled(bool)), addButton_, SLOT(setEnabled(bool)) );

    QHBoxLayout *addLayout = new QHBoxLayout();
    addLayout->addWidget(pvNameLineEdit_);
    addLayout->addWidget(addButton_);

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

    QHBoxLayout *timeLayout = new QHBoxLayout();
    timeLayout->addWidget(timeLabel);
    timeLayout->addWidget(timeEntry);

    QGroupBox *timeGroup = new QGroupBox();
    timeGroup->setLayout(timeLayout);
    timeGroup->setFlat(true);
    timeGroup->setEnabled(false);

    message_ = new QLabel("");
    connect( this, SIGNAL(clearMessage()), message_, SLOT(clear()) );
    connect( this, SIGNAL(error(QString)), this, SLOT(displayMessage(QString)) );

    QVBoxLayout *controlsLayout = new QVBoxLayout();
    controlsLayout->addWidget(listView_);
    controlsLayout->addLayout(addLayout);
    controlsLayout->addWidget(pauseResumeGroup);
    controlsLayout->addWidget(timeGroup);
//    controlsLayout->addWidget(message_);

    QGroupBox *controlsGroup = new QGroupBox();
//    controlsGroup->setTitle("PV Controls");
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

    connect( model_, SIGNAL(pvValid(bool)), this, SLOT(resetControls()) );
    connect( model_, SIGNAL(errorMessage(QString)), this, SLOT(displayMessage(QString)) );

    listView_->setPVModel(model_);
}



void StripToolQuickControls::resetControls()
{
    emit clearName();
    emit buttonEnabled(true);
    emit nameEntryEnabled(true);
    emit nameEntryFocus();
}



void StripToolQuickControls::displayMessage(const QString &text)
{
    message_->setText(text);
    emit reset();
}



void StripToolQuickControls::addClicked()
{
    emit buttonEnabled(false);
    emit nameEntryEnabled(false);

    QString pvName = pvNameLineEdit_->text();

    if (pvName == "")
    {
        emit error("PV name cannot be blank.");

    } else {

        emit addPV(pvName);
    }
}



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
