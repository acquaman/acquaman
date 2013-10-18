#include "AddNewPVsDialog.h"

AddNewPVsDialog::AddNewPVsDialog(QWidget *parent) : QDialog(parent)
{
    pvFromInfoTab_ = new AddPVFromInfoTab();
    connect(pvFromInfoTab_, SIGNAL(enableButtonBox(bool)), buttonBox_, SLOT(setEnabled(bool)) );

    tabWidget_ = new QTabWidget(this);
    tabWidget_->addTab(pvFromInfoTab_, tr("From Info"));

    QPushButton *acceptButton = new QPushButton("Accept");
    acceptButton->setDefault(true);
    acceptButton->setEnabled(false);

    QPushButton *cancelButton = new QPushButton("Cancel");

    buttonBox_ = new QDialogButtonBox(Qt::Horizontal, this);
    buttonBox_->addButton(acceptButton, QDialogButtonBox::AcceptRole);
    buttonBox_->addButton(cancelButton, QDialogButtonBox::RejectRole);
    connect(buttonBox_, SIGNAL(accepted()), this, SLOT(accept()) );
    connect(buttonBox_, SIGNAL(rejected()), this, SLOT(reject()) );

    QVBoxLayout *dialogLayout = new QVBoxLayout();
    dialogLayout->addWidget(tabWidget_);
    dialogLayout->addWidget(buttonBox_);

    setModal(true);
    setLayout(dialogLayout);
    setWindowTitle(tr("Add New PVs to Plot"));
}



AddNewPVsDialog::~AddNewPVsDialog()
{
}



QString AddNewPVsDialog::pvName()
{
    return pvFromInfoTab_->pvName();
}



QString AddNewPVsDialog::pvDescription()
{
    return pvFromInfoTab_->pvDescription();
}



QString AddNewPVsDialog::pvUnits()
{
    return pvFromInfoTab_->pvUnits();
}
