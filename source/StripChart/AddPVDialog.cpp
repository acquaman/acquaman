#include "StripChart/AddPVDialog.h"

AddPVDialog::AddPVDialog(QWidget *parent) :
    QDialog(parent)
{
    addPVDialog_ = new QDialog(this);
    connect( addPVDialog_, SIGNAL(closeDialog()), this, SLOT(onCloseDialogEmitted()) );

    pvNamePrompt_ = new QLabel("PV name : ");
    pvNameLineEdit_ = new QLineEdit();
    pvNamePrompt_->setBuddy(pvNameLineEdit_);
    connect( pvNameLineEdit_, SIGNAL(editingFinished()), this, SLOT(enableAcceptButton()) );

    pvValidLabel_ = new QLabel();
    pvValidLabel_->setFont(QFont("Arial", 8));

    QGridLayout *pvNameEntry = new QGridLayout();
    pvNameEntry->addWidget(pvNamePrompt_, 0, 0);
    pvNameEntry->addWidget(pvNameLineEdit_, 0, 1);
    pvNameEntry->addWidget(pvValidLabel_, 1, 1);

    acceptButton_ = new QPushButton("Accept");
    acceptButton_->setEnabled(false);
    connect( acceptButton_, SIGNAL(clicked()), this, SLOT(onAcceptButtonClicked()) );

    cancelButton_ = new QPushButton("Cancel");
    cancelButton_->setEnabled(true);
    connect( cancelButton_, SIGNAL(clicked()), this, SLOT(onCancelButtonClicked()) );

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(acceptButton_);
    buttonLayout->addWidget(cancelButton_);

    QVBoxLayout *windowLayout = new QVBoxLayout();
    windowLayout->addLayout(pvNameEntry);
    windowLayout->addLayout(buttonLayout);

    addPVDialog_->setModal(true);
    addPVDialog_->setLayout(windowLayout);
    addPVDialog_->setWindowTitle("Add PV to Plot");
    addPVDialog_->show();
}


void AddPVDialog::enableAcceptButton()
{
    //  save the pv name entered.
    pvNameEntered_ = pvNameLineEdit_->text();

    //  the pv name entered can only be accepted if the field is not empty!
    if (pvNameEntered_.isEmpty())
        acceptButton_->setEnabled(false);
    else
        acceptButton_->setEnabled(true);
}


void AddPVDialog::onAcceptButtonClicked()
{
    //  attempt to connect to the given pv.
    pvControl_ = new AMReadOnlyPVControl(pvNameEntered_, pvNameEntered_, this);
    connect( pvControl_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)) );
}


void AddPVDialog::onPVConnected(bool isConnected)
{
    //  if the pv can be connected to, it's valid! Yay! Else, let user know.
    if(isConnected)
    {
        newActivePVName_ = pvNameEntered_;
        emit closeDialog();

    } else {
        pvValidLabel_->setText("PV not found.");
    }
}


void AddPVDialog::onCancelButtonClicked()
{
    emit closeDialog();
}

void AddPVDialog::onCloseDialogEmitted()
{
    close();
}


