#include "StripChart/AddPVFromInfoTab.h"

AddPVFromInfoTab::AddPVFromInfoTab(QWidget *parent) : QWidget(parent)
{
    QLabel *pvNamePrompt = new QLabel("PV name : ");
    pvNamePrompt->setToolTip("Enter the EPICS read PV name.");

    QLineEdit *pvNameLineEdit = new QLineEdit();
    connect( pvNameLineEdit, SIGNAL(textEdited(QString)), this, SLOT(onPVNameEntered(QString)) );
    connect( this, SIGNAL(enablePVName(bool)), pvNameLineEdit, SLOT(setEnabled(bool)) );

    QLabel *pvDescriptionPrompt = new QLabel("Description : ");
    pvDescriptionPrompt->setToolTip("Enter the name that will be used to identify this PV on the plot.");

    QLineEdit *pvDescriptionLineEdit = new QLineEdit();
    connect( pvDescriptionLineEdit, SIGNAL(textEdited(QString)), this, SLOT(onPVDescriptionEntered(QString)) );
    connect( this, SIGNAL(enablePVDescription(bool)), pvDescriptionLineEdit, SLOT(setEnabled(bool)) );

    QLabel *pvUnitsPrompt = new QLabel("Units : ");
    pvUnitsPrompt->setToolTip("Enter the units of this PV.");

    QLineEdit *pvUnitsLineEdit = new QLineEdit();
    connect( pvUnitsLineEdit, SIGNAL(textEdited(QString)), this, SLOT(onPVUnitsEntered(QString)) );
    connect( this, SIGNAL(enablePVUnits(bool)), pvUnitsLineEdit, SLOT(setEnabled(bool)) );

    pvValidMessage_ = new QLabel();

    QGridLayout *pvEntryLayout = new QGridLayout();
    pvEntryLayout->addWidget(pvNamePrompt, 0, 0);
    pvEntryLayout->addWidget(pvNameLineEdit, 0, 1);
    pvEntryLayout->addWidget(pvDescriptionPrompt, 1, 0);
    pvEntryLayout->addWidget(pvDescriptionLineEdit, 1, 1);
    pvEntryLayout->addWidget(pvUnitsPrompt, 2, 0);
    pvEntryLayout->addWidget(pvUnitsLineEdit, 2, 1);
    pvEntryLayout->addWidget(pvValidMessage_, 3, 1);

    QVBoxLayout *windowLayout = new QVBoxLayout();
    windowLayout->addLayout(pvEntryLayout);

    connect( this, SIGNAL(enablePVEntry(bool)), this, SLOT(setPVEntryEnabled(bool)) );
}



AddPVFromInfoTab::~AddPVFromInfoTab()
{
}



QString AddPVFromInfoTab::pvName()
{
    return pvName_;
}



QString AddPVFromInfoTab::pvDescription()
{
    return pvDescription_;
}



QString AddPVFromInfoTab::pvUnits()
{
    return pvUnits_;
}



void AddPVFromInfoTab::checkValidity(const QString &pvName)
{
    emit enablePVEntry(false);

    if (pvName != "")
    {
        //  reset validity warning and attempt to connect to the pv.
        pvValidMessage_->setText(" ");
        AMReadOnlyPVControl *pvControl = new AMReadOnlyPVControl(pvName, pvName, this);
        connect( pvControl, SIGNAL(connected(bool)), this, SLOT(pvIsValid(bool)) );

    } else {
        emit pvIsValid(false);
    }
}



void AddPVFromInfoTab::onPVNameEntered(const QString &name)
{
    pvName_ = name;
    checkValidity(name);
}



void AddPVFromInfoTab::onPVDescriptionEntered(const QString &description)
{
    pvDescription_ = description;
}



void AddPVFromInfoTab::onPVUnitsEntered(const QString &units)
{
    pvUnits_ = units;
}



void AddPVFromInfoTab::setPVEntryEnabled(bool isEnabled)
{
    emit enablePVName(isEnabled);
    emit enablePVDescription(isEnabled);
    emit enablePVUnits(isEnabled);
}



void AddPVFromInfoTab::pvIsValid(bool isValid)
{
    if (isValid) {
        emit enablePVEntry(true);
        emit enableButtonBox(true);

    } else {

        pvValidMessage_->setText("Invalid PV name.");
        pvName_ = "";
        pvDescription_ = "";
        pvUnits_ = "";

        emit enablePVEntry(true);
    }
}
