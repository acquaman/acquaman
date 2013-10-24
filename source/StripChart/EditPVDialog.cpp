#include "EditPVDialog.h"

EditPVDialog::EditPVDialog(const QString &pvName, QWidget *parent) :
    QDialog(parent)
{
    points_ = 0;

    QLabel *nameLabel = new QLabel("Name : ");
    QLabel *nameEntry_ = new QLabel(pvName);

    QLabel *descriptionLabel = new QLabel("Description : ");
    descriptionEntry_ = new QLineEdit();
    descriptionLabel->setBuddy(descriptionEntry_);
    connect( descriptionEntry_, SIGNAL(textChanged(QString)), this, SLOT(descriptionEntered(QString)) );

    QLabel *unitsLabel = new QLabel("Units : ");
    unitsEntry_ = new QLineEdit();
    unitsLabel->setBuddy(unitsEntry_);
    connect( unitsEntry_, SIGNAL(textChanged(QString)), this, SLOT(unitsEntered(QString)) );

    QLabel *pointsLabel = new QLabel("Points # : ");
    pointsEntry_ = new QLineEdit();
    pointsLabel->setBuddy(pointsEntry_);
    connect( pointsEntry_, SIGNAL(textChanged(QString)), this, SLOT(pointsEntered(QString)) );

    QGridLayout *entryLayout = new QGridLayout();
    entryLayout->addWidget(nameLabel, 0, 0);
    entryLayout->addWidget(nameEntry_, 0, 1);
    entryLayout->addWidget(descriptionLabel, 1, 0);
    entryLayout->addWidget(descriptionEntry_, 1, 1);
    entryLayout->addWidget(unitsLabel, 2, 0);
    entryLayout->addWidget(unitsEntry_, 2, 1);
    entryLayout->addWidget(pointsLabel, 3, 0);
    entryLayout->addWidget(pointsEntry_, 3, 1);

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



int EditPVDialog::points()
{
        return points_;
}



void EditPVDialog::descriptionEntered(const QString &newDescription)
{
    description_ = newDescription;
}



void EditPVDialog::unitsEntered(const QString &newUnits)
{
    units_ = newUnits;
}



void EditPVDialog::pointsEntered(const QString &points)
{
    points_ = points.toInt();
}
