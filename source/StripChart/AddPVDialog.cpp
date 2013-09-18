#include "StripChart/AddPVDialog.h"

AddPVDialog::AddPVDialog(QWidget *parent) :
    QDialog(parent)
{
    //  initialize the strings that will contain accepted pv info!
    acceptedPVName_ = "";
    acceptedPVDescription_ = "";

    //  begin building dialog widgets.
    pvNamePrompt_ = new QLabel("PV name : ");
    pvNameLineEdit_ = new QLineEdit();
    connect( pvNameLineEdit_, SIGNAL(editingFinished()), this, SLOT(onPVNameEntered()) );
    connect( this, SIGNAL(enablePVName(bool)), pvNameLineEdit_, SLOT(setEnabled(bool)) );

    pvDescriptionPrompt_ = new QLabel("Description : ");
    pvDescriptionLineEdit_ = new QLineEdit();
    connect( this, SIGNAL(enablePVDescription(bool)), pvDescriptionLineEdit_, SLOT(setEnabled(bool)) );

    pvValidLabel_ = new QLabel();

    QGridLayout *pvEntryLayout = new QGridLayout();
    pvEntryLayout->addWidget(pvNamePrompt_, 0, 0);
    pvEntryLayout->addWidget(pvNameLineEdit_, 0, 1);
    pvEntryLayout->addWidget(pvDescriptionPrompt_, 1, 0);
    pvEntryLayout->addWidget(pvDescriptionLineEdit_, 1, 1);
    pvEntryLayout->addWidget(pvValidLabel_, 2, 1);

    acceptButton_ = new QPushButton("Accept");
    acceptButton_->setEnabled(false);
    connect( this, SIGNAL(enableAcceptButton(bool)), acceptButton_, SLOT(setEnabled(bool)) );
    connect( acceptButton_, SIGNAL(clicked()), this, SLOT(onAcceptButtonClicked()) );

    cancelButton_ = new QPushButton("Cancel");
    cancelButton_->setEnabled(true);
    connect( this, SIGNAL(enableCancelButton(bool)), cancelButton_, SLOT(setEnabled(bool)) );
    connect( cancelButton_, SIGNAL(clicked()), this, SLOT(onCancelButtonClicked()) );

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(acceptButton_);
    buttonLayout->addWidget(cancelButton_);

    QVBoxLayout *windowLayout = new QVBoxLayout();
    windowLayout->addLayout(pvEntryLayout);
    windowLayout->addLayout(buttonLayout);

    connect(this, SIGNAL(newPVIsValid()), this, SLOT(onNewPVIsValid()) );

    setModal(true);
    setLayout(windowLayout);
    setWindowTitle("Add PV to Plot");
    resize(350, 100);

}


void AddPVDialog::onPVNameEntered()
{
    //  extract new text entered by user, and reset the pv validity warning.
    pvNameEntered_ = pvNameLineEdit_->text();
    pvDescriptionEntered_ = pvDescriptionLineEdit_->text();

    pvValidLabel_->setText("");

    //  check that the user entered all the information!
    if (pvNameEntered_.isEmpty() || pvDescriptionEntered_.isEmpty())
        emit enableAcceptButton(false);
    else
        emit enableAcceptButton(true);
}

void AddPVDialog::onAcceptButtonClicked()
{
    //  disable buttons and fields while we wait to see if pv is valid.
    emit enablePVName(false);
    emit enablePVDescription(false);
    emit enableAcceptButton(false);
    emit enableCancelButton(false);

    //  attempt to connect to the given pv to test for validity.
    pvControl_ = new AMReadOnlyPVControl(pvDescriptionEntered_, pvNameEntered_, this);
    connect( pvControl_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)) );
}


void AddPVDialog::onPVConnected(const bool isConnected)
{
    //  if the pv can be connected to, it's valid! Yay! Else, let user know.
    if(isConnected) {
        emit newPVIsValid();

    } else {
        pvValidLabel_->setText("PV name not found.");

        emit enablePVName(true);
        emit enablePVDescription(true);
        emit enableAcceptButton(false);
        emit enableCancelButton(true);
    }
}


void AddPVDialog::onNewPVIsValid()
{
    acceptedPVName_ = pvNameEntered_;
    acceptedPVDescription_ = pvDescriptionEntered_;

    emit newPVAccepted(acceptedPVName_, acceptedPVDescription_);
    emit accept();
}


void AddPVDialog::onCancelButtonClicked()
{
    emit reject();
}


