#include "StripChart/AddPVDialog.h"

AddPVDialog::AddPVDialog(QWidget *parent) :
    QDialog(parent)
{
    QLabel *pvNamePrompt = new QLabel("PV name : ");
    pvNamePrompt->setToolTip("Enter the EPICS read PV name for the values you wish to view.");
    QLineEdit *pvNameLineEdit_ = new QLineEdit();
    connect( pvNameLineEdit_, SIGNAL(textEdited(QString)), this, SLOT(onPVNameEntered(QString)) );
    connect( this, SIGNAL(enablePVName(bool)), pvNameLineEdit_, SLOT(setEnabled(bool)) );

    QLabel *pvDescriptionPrompt = new QLabel("Description : ");
    pvDescriptionPrompt->setToolTip("Enter the name that will be used to identify this PV on the graph.");
    QLineEdit *pvDescriptionLineEdit_ = new QLineEdit();
    connect( pvDescriptionLineEdit_, SIGNAL(textEdited(QString)), this, SLOT(onPVDescriptionEntered(QString)) );
    connect( this, SIGNAL(enablePVDescription(bool)), pvDescriptionLineEdit_, SLOT(setEnabled(bool)) );

    QLabel *pvUnitsPrompt = new QLabel("Units : ");
    pvUnitsPrompt->setToolTip("Enter the units of this PV.");
    QLineEdit *pvUnitsLineEdit_ = new QLineEdit();
    connect( pvUnitsLineEdit_, SIGNAL(textEdited(QString)), this, SLOT(onPVUnitsEntered(QString)) );
    connect( this, SIGNAL(enablePVUnits(bool)), pvUnitsLineEdit_, SLOT(setEnabled(bool)) );

    pvValidMessage_ = new QLabel();

    QGridLayout *pvEntryLayout = new QGridLayout();
    pvEntryLayout->addWidget(pvNamePrompt, 0, 0);
    pvEntryLayout->addWidget(pvNameLineEdit_, 0, 1);
    pvEntryLayout->addWidget(pvDescriptionPrompt, 1, 0);
    pvEntryLayout->addWidget(pvDescriptionLineEdit_, 1, 1);
    pvEntryLayout->addWidget(pvUnitsPrompt, 2, 0);
    pvEntryLayout->addWidget(pvUnitsLineEdit_, 2, 1);
    pvEntryLayout->addWidget(pvValidMessage_, 3, 1);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect( this, SIGNAL(enableButtonBox(bool)), buttonBox, SLOT(setEnabled(bool)) );
    connect( buttonBox, SIGNAL(accepted()), this, SLOT(onAcceptButtonClicked()) );
    connect( buttonBox, SIGNAL(rejected()), this, SLOT(reject()) );

    QVBoxLayout *windowLayout = new QVBoxLayout();
    windowLayout->addLayout(pvEntryLayout);
    windowLayout->addWidget(buttonBox);

    connect(this, SIGNAL(newPVIsValid()), this, SLOT(onNewPVIsValid()) );

    setModal(true);
    setLayout(windowLayout);
    setWindowTitle("Add PV to Plot");
    resize(350, 100);

}


void AddPVDialog::onPVNameEntered(const QString name)
{
    //  extract new text entered by user, and reset the pv validity warning.
    pvName_ = name;
    pvValidMessage_->setText("");
}


void AddPVDialog::onPVDescriptionEntered(const QString description)
{
    pvDescription_ = description;
}


void AddPVDialog::onPVUnitsEntered(const QString units)
{
    pvUnits_ = units;
}

void AddPVDialog::onAcceptButtonClicked()
{
    //  disable buttons and fields while we wait to see if pv is valid.
    emit enablePVName(false);
    emit enablePVDescription(false);
    emit enableButtonBox(false);
    emit enablePVUnits(false);

    //  attempt to connect to the given pv to test for validity.
    AMReadOnlyPVControl *pvControl = new AMReadOnlyPVControl(pvDescription_, pvName_, this);
    connect( pvControl, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)) );
}


void AddPVDialog::onPVConnected(bool isConnected)
{
    //  if the pv can be connected to, it's valid! Yay! Else, let user know.
    if(isConnected) {
        emit newPVIsValid();

    } else {

        pvValidMessage_->setText("Invalid PV name.");
        pvName_ = "";
        pvDescription_ = "";
        pvUnits_ = "";

        emit enablePVName(true);
        emit enablePVDescription(true);
        emit enableButtonBox(true);
        emit enablePVUnits(true);
    }
}


void AddPVDialog::onNewPVIsValid()
{
    emit newPVAccepted(pvName_, pvDescription_, pvUnits_);
    emit accept();
}



