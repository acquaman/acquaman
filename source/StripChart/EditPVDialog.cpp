#include "EditPVDialog.h"

EditPVDialog::EditPVDialog(QStringList pvInfo, QWidget *parent) :
    QDialog(parent)
{
    descriptionChanged_ = false;
    unitsChanged_ = false;
    granChanged_ = false;
    colorChanged_ = false;
    yMaxChanged_ = false;
    yMinChanged_ = false;

    QGridLayout *entryLayout = new QGridLayout();
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    QVBoxLayout *dialogLayout = new QVBoxLayout();
    dialogLayout->addLayout(entryLayout);
    dialogLayout->addLayout(buttonLayout);

    setWindowTitle("Edit PV Info");
    setLayout(dialogLayout);
    setModal(true);


    ////////////////////////////////////////////////////////

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
    connect( descriptionEntry_, SIGNAL(textEdited(QString)), this, SLOT(descriptionEntered(QString)) );
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
//        displayMaxEntry_->setText(displayedMax);
        maxLabel->setBuddy(displayMaxEntry_);
        connect( displayMaxEntry_, SIGNAL(textChanged(QString)), this, SLOT(displayMaxEntered(QString)) );
        entryLayout->addWidget(displayMaxEntry_, 6, 1);

        currentDisplayMax_ = new QLabel("boo");
        entryLayout->addWidget(currentDisplayMax_, 6, 2);

        QLabel *minLabel = new QLabel("Displayed min : ");
        entryLayout->addWidget(minLabel, 7, 0);

        QString displayedMin = pvInfo.at(7);
        displayMinEntry_ = new QLineEdit();
//        displayMinEntry_->setText(displayedMin);
        minLabel->setBuddy(displayMinEntry_);
        connect( displayMinEntry_, SIGNAL(textChanged(QString)), this, SLOT(displayMinEntered(QString)) );
        entryLayout->addWidget(displayMinEntry_, 7, 1);

        currentDisplayMin_ = new QLabel("ger");
        entryLayout->addWidget(currentDisplayMin_, 7, 2);
    }


    okButton_ = new QPushButton("Ok");
    okButton_->setDefault(true);
    connect( okButton_, SIGNAL(clicked()), this, SLOT(accept()) );
    buttonLayout->addWidget(okButton_);

    cancelButton_ = new QPushButton("Cancel");
    connect( cancelButton_, SIGNAL(clicked()), this, SLOT(reject()) );
    buttonLayout->addWidget(cancelButton_);

}



EditPVDialog::~EditPVDialog()
{
}



bool EditPVDialog::descriptionChanged()
{
    return descriptionChanged_;
}



QString EditPVDialog::description()
{
    return description_;
}



bool EditPVDialog::unitsChanged()
{
    return unitsChanged_;
}



QString EditPVDialog::units()
{
    return units_;
}



bool EditPVDialog::granularityChanged()
{
    return granChanged_;
}



int EditPVDialog::granularity()
{
    return gran_;
}



bool EditPVDialog::colorChanged()
{
    return colorChanged_;
}



QString EditPVDialog::color()
{
    return color_;
}



bool EditPVDialog::displayMaxChanged()
{
    return yMaxChanged_;
}


QString EditPVDialog::displayMax()
{
    return displayMax_;
}


bool EditPVDialog::displayMinChanged()
{
    return yMinChanged_;
}



QString EditPVDialog::displayMin()
{
    return displayMin_;
}



void EditPVDialog::descriptionEntered(const QString &newDescription)
{
    description_ = newDescription;
    descriptionChanged_ = true;
}



void EditPVDialog::unitsEntered(const QString &newUnits)
{
    units_ = newUnits;
    unitsChanged_ = true;
}



void EditPVDialog::granularityEntered(const QString &gran)
{
    gran_ = gran.toInt();
    granChanged_ = true;
}


void EditPVDialog::colorEntered(const QString &newColor)
{
    color_ = newColor;
    colorChanged_ = true;
}


void EditPVDialog::displayMaxEntered(const QString &max)
{
    displayMax_ = max;
    yMaxChanged_ = true;
}


void EditPVDialog::displayMinEntered(const QString &min)
{
    displayMin_ = min;
    yMinChanged_ = true;
}


void EditPVDialog::toUpdateCurrentDisplayMax(double currentMax)
{
    currentDisplayMax_->setText(QString::number(currentMax));
}


void EditPVDialog::toUpdateCurrentDisplayMin(double currentMin)
{
    currentDisplayMin_->setText(QString::number(currentMin));
}
