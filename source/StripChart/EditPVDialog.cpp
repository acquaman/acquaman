#include "EditPVDialog.h"

EditPVDialog::EditPVDialog(QStringList pvInfo, QStringList pvDefaults, QWidget *parent) :
    QDialog(parent)
{
    defaults_ = pvDefaults;

    description_ = 0;
    units_ = 0;
    gran_ = 0;
    color_ = 0;
    displayMax_ = 0;
    displayMin_ = 0;

    QGridLayout *entryLayout = new QGridLayout();
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    QVBoxLayout *dialogLayout = new QVBoxLayout();
    dialogLayout->addLayout(entryLayout);
    dialogLayout->addLayout(buttonLayout);

    setWindowTitle("Edit PV Info");
    setLayout(dialogLayout);
    setModal(true);


    ////////////////////////////////////////////////////////

//    QGridLayout *basicInfoLayout = new QGridLayout();
//    QGroupBox *basicGroup = new QGroupBox();
//    basicGroup->setLayout(basicInfoLayout);
//    basicGroup->setFlat(true);

    QString pvName = pvInfo.at(0);
    QLabel *nameLabel = new QLabel("Name : ");
    QLabel *nameEntry = new QLabel(pvName);
    entryLayout->addWidget(nameLabel, 0, 0);
    entryLayout->addWidget(nameEntry, 0, 1);

    QString pvDateAdded = pvInfo.at(1);
    QLabel *dateTimeLabel = new QLabel("Date added : ");
    QLabel *dateTime = new QLabel(pvDateAdded);
    entryLayout->addWidget(dateTimeLabel, 1, 0);
    entryLayout->addWidget(dateTime, 1, 1);

    QString pvDescription = pvInfo.at(2);
    QLabel *descriptionLabel = new QLabel("Description : ");
    descriptionEntry_ = new QLineEdit();
    descriptionEntry_->setText(pvDescription);
    descriptionLabel->setBuddy(descriptionEntry_);
    connect( descriptionEntry_, SIGNAL(textChanged(QString)), this, SLOT(descriptionEntered(QString)) );
    entryLayout->addWidget(descriptionLabel, 2, 0);
    entryLayout->addWidget(descriptionEntry_, 2, 1);

    QString pvUnits = pvInfo.at(3);
    QLabel *unitsLabel = new QLabel("Units : ");
    unitsEntry_ = new QLineEdit();
    unitsEntry_->setText(pvUnits);
    unitsLabel->setBuddy(unitsEntry_);
    connect( unitsEntry_, SIGNAL(textChanged(QString)), this, SLOT(unitsEntered(QString)) );
    entryLayout->addWidget(unitsLabel, 3, 0);
    entryLayout->addWidget(unitsEntry_, 3, 1);

    QString pvGranularity = pvInfo.at(4);
    QLabel *granularityLabel = new QLabel("Granularity :");
    granularityEntry_ = new QLineEdit();
    granularityEntry_->setText(pvGranularity);
    granularityLabel->setBuddy(granularityEntry_);
    connect( granularityEntry_, SIGNAL(textChanged(QString)), this, SLOT(granularityEntered(QString)) );
    entryLayout->addWidget(granularityLabel, 4, 0);
    entryLayout->addWidget(granularityEntry_, 4, 1);

    QString pvColorHex = pvInfo.at(5);
    QLabel *colorLabel = new QLabel("Color : ");
    colorEntry_ = new QLineEdit();
    colorEntry_->setText(pvColorHex);
    colorLabel->setBuddy(colorEntry_);
    connect( colorEntry_, SIGNAL(textChanged(QString)), this, SLOT(colorEntered(QString)) );
    entryLayout->addWidget(colorLabel, 5, 0);
    entryLayout->addWidget(colorEntry_, 5, 1);

    if (pvInfo.size() > 6) {
        QLabel *maxLabel = new QLabel("Displayed max : ");
        entryLayout->addWidget(maxLabel, 6, 0);

        QString displayedMax = pvInfo.at(6);
        displayMaxEntry_ = new QLineEdit();
        displayMaxEntry_->setText(displayedMax);
        maxLabel->setBuddy(displayMaxEntry_);
        connect( displayMaxEntry_, SIGNAL(textChanged(QString)), this, SLOT(displayMaxEntered(QString)) );
        entryLayout->addWidget(displayMaxEntry_, 6, 1);

        QPushButton *restoreMaxDisplay = new QPushButton("Restore");
        restoreMaxDisplay->setDefault(false);
        connect( restoreMaxDisplay, SIGNAL(clicked()), this, SLOT(toRestoreMaxDisplay()) );
        entryLayout->addWidget(restoreMaxDisplay, 6, 2);

        QLabel *minLabel = new QLabel("Displayed min : ");
        entryLayout->addWidget(minLabel, 7, 0);

        QString displayedMin = pvInfo.at(7);
        displayMinEntry_ = new QLineEdit();
        displayMinEntry_->setText(displayedMin);
        minLabel->setBuddy(displayMinEntry_);
        connect( displayMinEntry_, SIGNAL(textChanged(QString)), this, SLOT(displayMinEntered(QString)) );
        entryLayout->addWidget(displayMinEntry_, 7, 1);

        QPushButton *restoreMinDisplay = new QPushButton("Restore");
        restoreMinDisplay->setDefault(false);
        connect( restoreMinDisplay, SIGNAL(clicked()), this, SLOT(toRestoreMinDisplay()) );
        entryLayout->addWidget(restoreMinDisplay, 7, 2);
    }


    okButton_ = new QPushButton("Ok");
    okButton_->setDefault(true);
    connect( okButton_, SIGNAL(clicked()), this, SLOT(accept()) );
    buttonLayout->addWidget(okButton_);

    cancelButton_ = new QPushButton("Cancel");
    connect( cancelButton_, SIGNAL(clicked()), this, SLOT(reject()) );
    buttonLayout->addWidget(cancelButton_);

    restoreButton_ = new QPushButton("Restore");
    restoreButton_->setToolTip("Restore default settings");
    connect( restoreButton_, SIGNAL(clicked()), this, SLOT(restore()) );
}



EditPVDialog::~EditPVDialog()
{
}



QString* EditPVDialog::description()
{
    return description_;
}



QString* EditPVDialog::units()
{
    return units_;
}



int* EditPVDialog::granularity()
{
    return gran_;
}



QString* EditPVDialog::color()
{
    return color_;
}


double* EditPVDialog::displayMax()
{
    return displayMax_;
}



double* EditPVDialog::displayMin()
{
    return displayMin_;
}



void EditPVDialog::descriptionEntered(QString &newDescription)
{
    description_ = &newDescription;
}



void EditPVDialog::unitsEntered(QString &newUnits)
{
    units_ = &newUnits;
}



void EditPVDialog::granularityEntered(QString &gran)
{
    int newGranularity = gran.toInt();
    gran_ = &newGranularity;
}


void EditPVDialog::colorEntered(QString &newColor)
{
    color_ = &newColor;
}


void EditPVDialog::displayMaxEntered(QString &max)
{
    double newMax = max.toDouble();
    displayMax_ = &newMax;
}


void EditPVDialog::displayMinEntered(QString &min)
{
    double newMin = min.toDouble();
    displayMin_ = &newMin;
}


void EditPVDialog::toRestoreMaxDisplay()
{
    displayMax_ = 0;
    displayMaxEntry_->setText("");
}


void EditPVDialog::toRestoreMinDisplay()
{
    displayMin_ = 0;
    displayMinEntry_->setText("");
}


void EditPVDialog::restore()
{
//    emit restoreSelectedDefaults();
    emit accept();
}
