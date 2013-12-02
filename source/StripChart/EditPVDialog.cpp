#include "EditPVDialog.h"

EditPVDialog::EditPVDialog(QStringList pvInfo, QWidget *parent) :
    QDialog(parent)
{
    QString pvName = pvInfo.at(0);
    QString pvDescription = pvInfo.at(1);
    QString pvUnits = pvInfo.at(2);
    // we don't show color here, which would be pvInfo.at(3)!
    QString pvGranularity = pvInfo.at(4);

    QLabel *nameLabel = new QLabel("Name : ");
    QLabel *nameEntry = new QLabel(pvName);

    QLabel *descriptionLabel = new QLabel("Description : ");
    descriptionEntry_ = new QLineEdit();
    descriptionEntry_->setText(pvDescription);
    descriptionLabel->setBuddy(descriptionEntry_);
    connect( descriptionEntry_, SIGNAL(textChanged(QString)), this, SLOT(descriptionEntered(QString)) );

    QLabel *unitsLabel = new QLabel("Units : ");
    unitsEntry_ = new QLineEdit();
    unitsEntry_->setText(pvUnits);
    unitsLabel->setBuddy(unitsEntry_);
    connect( unitsEntry_, SIGNAL(textChanged(QString)), this, SLOT(unitsEntered(QString)) );

    QLabel *granularityLabel = new QLabel("Granularity :");
    granularityEntry_ = new QLineEdit();
    granularityEntry_->setText(pvGranularity);
    granularityLabel->setBuddy(granularityEntry_);
    connect(granularityEntry_, SIGNAL(textChanged(QString)), this, SLOT(granularityEntered(QString)) );

    QGridLayout *entryLayout = new QGridLayout();
    entryLayout->addWidget(nameLabel, 0, 0);
    entryLayout->addWidget(nameEntry, 0, 1);
    entryLayout->addWidget(descriptionLabel, 1, 0);
    entryLayout->addWidget(descriptionEntry_, 1, 1);
    entryLayout->addWidget(unitsLabel, 2, 0);
    entryLayout->addWidget(unitsEntry_, 2, 1);
    entryLayout->addWidget(granularityLabel, 3, 0);
    entryLayout->addWidget(granularityEntry_, 3, 1);

    QPushButton *okButton_ = new QPushButton("Ok");
    connect( okButton_, SIGNAL(clicked()), this, SLOT(accept()) );

    QPushButton *cancelButton = new QPushButton("Cancel");
    connect( cancelButton, SIGNAL(clicked()), this, SLOT(reject()) );

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(okButton_);
    buttonLayout->addWidget(cancelButton);

    QVBoxLayout *dialogLayout = new QVBoxLayout();
    dialogLayout->addLayout(entryLayout);
    dialogLayout->addLayout(buttonLayout);

    setWindowTitle("Edit PV Info");
    setLayout(dialogLayout);
    setModal(true);
}



EditPVDialog::~EditPVDialog()
{
}



QString EditPVDialog::description()
{
    return description_;
}



QString EditPVDialog::units()
{
    return units_;
}



int EditPVDialog::granularity()
{
    return gran_;
}



void EditPVDialog::descriptionEntered(const QString &newDescription)
{
    description_ = newDescription;
}



void EditPVDialog::unitsEntered(const QString &newUnits)
{
    units_ = newUnits;
}



void EditPVDialog::granularityEntered(const QString &gran)
{
    gran_ = gran.toInt();
}
