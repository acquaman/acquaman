#include "EditPVDialog.h"

EditPVDialog::EditPVDialog(StripToolBasicVariableInfo *info, QWidget *parent) :
    QDialog(parent)
{
//    info_ = info;

//    descriptionChanged_ = false;
//    unitsChanged_ = false;
//    granChanged_ = false;
//    colorChanged_ = false;
//    yMaxChanged_ = false;
//    yMinChanged_ = false;

//    QVBoxLayout *dialogLayout = new QVBoxLayout();
//    dialogLayout->addLayout(createFormGui());
//    dialogLayout->addLayout(createButtonGui());

//    setLayout(dialogLayout);
//    setWindowTitle("Edit Info");
//    setModal(true);
}



//QLayout* EditPVDialog::createFormGui()
//{
//    QGridLayout *formLayout = new QGridLayout();

//    QString pvName = info_->name();
//    QLabel *nameLabel = new QLabel("Name : ");
//    QLabel *nameEntry = new QLabel(pvName);
//    formLayout->addWidget(nameLabel, 0, 0);
//    formLayout->addWidget(nameEntry, 0, 1);

//    QString pvDateAdded = info_->creationDateTime();
//    QLabel *dateTimeLabel = new QLabel("Date added : ");
//    QLabel *dateTime = new QLabel(pvDateAdded);
//    formLayout->addWidget(dateTimeLabel, 1, 0);
//    formLayout->addWidget(dateTime, 1, 1);

//    QString pvDescription = info_->description();
//    QLabel *descriptionLabel = new QLabel("Description : ");
//    descriptionEntry_ = new QLineEdit();
//    descriptionEntry_->setText(pvDescription);
//    descriptionLabel->setBuddy(descriptionEntry_);
//    connect( descriptionEntry_, SIGNAL(textEdited(QString)), this, SLOT(descriptionEntered(QString)) );
//    formLayout->addWidget(descriptionLabel, 2, 0);
//    formLayout->addWidget(descriptionEntry_, 2, 1);

//    QString pvUnits = info_->units();
//    QLabel *unitsLabel = new QLabel("Units : ");
//    unitsEntry_ = new QLineEdit();
//    unitsEntry_->setText(pvUnits);
//    unitsLabel->setBuddy(unitsEntry_);
//    connect( unitsEntry_, SIGNAL(textChanged(QString)), this, SLOT(unitsEntered(QString)) );
//    formLayout->addWidget(unitsLabel, 3, 0);
//    formLayout->addWidget(unitsEntry_, 3, 1);

////    QString pvGranularity = info_->granularity();
////    QLabel *granularityLabel = new QLabel("Granularity :");
////    granularityEntry_ = new QLineEdit();
////    granularityEntry_->setText(pvGranularity);
////    granularityLabel->setBuddy(granularityEntry_);
////    connect( granularityEntry_, SIGNAL(textChanged(QString)), this, SLOT(granularityEntered(QString)) );
////    formLayout->addWidget(granularityLabel, 4, 0);
////    formLayout->addWidget(granularityEntry_, 4, 1);

////    QString pvColorHex = info_->colorName();
//    QString pvColorHex = "";
//    QLabel *colorLabel = new QLabel("Color : ");
//    colorEntry_ = new QLineEdit();
//    colorEntry_->setText(pvColorHex);
//    colorLabel->setBuddy(colorEntry_);
//    connect( colorEntry_, SIGNAL(textChanged(QString)), this, SLOT(colorEntered(QString)) );
//    formLayout->addWidget(colorLabel, 5, 0);
//    formLayout->addWidget(colorEntry_, 5, 1);

//    QLabel *maxLabel = new QLabel("Displayed max : ");
//    formLayout->addWidget(maxLabel, 6, 0);

////    QString displayedMax = pvInfo.at(6);
//    QString displayedMax = "";
//    displayMaxEntry_ = new QLineEdit();
//    displayMaxEntry_->setText(displayedMax);
//    maxLabel->setBuddy(displayMaxEntry_);
//    connect( displayMaxEntry_, SIGNAL(textChanged(QString)), this, SLOT(displayMaxEntered(QString)) );
//    formLayout->addWidget(displayMaxEntry_, 6, 1);

//    currentDisplayMax_ = new QLabel("---");
//    formLayout->addWidget(currentDisplayMax_, 6, 2);

//    automaticMax_ = new QCheckBox("Automatically update max");
//    formLayout->addWidget(automaticMax_, 7, 1);
//    connect( displayMaxEntry_, SIGNAL(textChanged(QString)), this, SLOT(setMaxCheckState(QString)) );
//    connect( automaticMax_, SIGNAL(stateChanged(int)), this, SLOT(toClearMaxEntry(int)) );
//    if (displayedMax == "")
//        automaticMax_->setCheckState(Qt::Checked);
//    else
//        automaticMax_->setCheckState(Qt::Unchecked);


//    QLabel *minLabel = new QLabel("Displayed min : ");
//    formLayout->addWidget(minLabel, 8, 0);

////    QString displayedMin = pvInfo.at(7);
//    QString displayedMin = "";
//    displayMinEntry_ = new QLineEdit();
//    displayMinEntry_->setText(displayedMin);
//    minLabel->setBuddy(displayMinEntry_);
//    connect( displayMinEntry_, SIGNAL(textChanged(QString)), this, SLOT(displayMinEntered(QString)) );
//    formLayout->addWidget(displayMinEntry_, 8, 1);

//    currentDisplayMin_ = new QLabel("---");
//    formLayout->addWidget(currentDisplayMin_, 8, 2);

//    automaticMin_ = new QCheckBox("Automatically update min");
//    formLayout->addWidget(automaticMin_, 9, 1);
//    connect( displayMinEntry_, SIGNAL(textChanged(QString)), this, SLOT(setMinCheckState(QString)) );
//    connect( automaticMin_, SIGNAL(stateChanged(int)), this, SLOT(toClearMinEntry(int)) );
//    if (displayedMin == "")
//        automaticMin_->setCheckState(Qt::Checked);
//    else
//        automaticMin_->setCheckState(Qt::Unchecked);


//    return formLayout;

//}



//QLayout* EditPVDialog::createButtonGui() {
//    QHBoxLayout *buttonLayout = new QHBoxLayout();

//    okButton_ = new QPushButton("Ok");
//    okButton_->setDefault(true);
//    connect( okButton_, SIGNAL(clicked()), this, SLOT(accept()) );
//    buttonLayout->addWidget(okButton_);

//    cancelButton_ = new QPushButton("Cancel");
//    connect( cancelButton_, SIGNAL(clicked()), this, SLOT(reject()) );
//    buttonLayout->addWidget(cancelButton_);

//    return buttonLayout;
//}



//EditPVDialog::~EditPVDialog()
//{
//}



//bool EditPVDialog::descriptionChanged()
//{
//    return descriptionChanged_;
//}



//QString EditPVDialog::description()
//{
//    return description_;
//}



//bool EditPVDialog::unitsChanged()
//{
//    return unitsChanged_;
//}



//QString EditPVDialog::units()
//{
//    return units_;
//}



//bool EditPVDialog::granularityChanged()
//{
//    return granChanged_;
//}



//QString EditPVDialog::granularity()
//{
//    return gran_;
//}



//bool EditPVDialog::colorChanged()
//{
//    return colorChanged_;
//}



//QString EditPVDialog::color()
//{
//    return color_;
//}



//bool EditPVDialog::displayMaxChanged()
//{
//    return yMaxChanged_;
//}


//QString EditPVDialog::displayMax()
//{
//    return displayMax_;
//}


//bool EditPVDialog::displayMinChanged()
//{
//    return yMinChanged_;
//}



//QString EditPVDialog::displayMin()
//{
//    return displayMin_;
//}



////bool EditPVDialog::shiftAmountChanged()
////{
////    return shiftAmountChanged_;
////}



////QString EditPVDialog::shiftAmount()
////{
////    return shiftAmount_;
////}



//void EditPVDialog::descriptionEntered(const QString &newDescription)
//{
//    description_ = newDescription;

//    if (description_ == "")
//        descriptionChanged_ = false;
//    else
//        descriptionChanged_ = true;
//}



//void EditPVDialog::unitsEntered(const QString &newUnits)
//{
//    units_ = newUnits;

//    if (units_ == "")
//        unitsChanged_ = false;
//    else
//        unitsChanged_ = true;
//}



//void EditPVDialog::granularityEntered(const QString &gran)
//{
//    gran_ = gran;

//    if (gran_ == "")
//        granChanged_ = false;
//    else
//        granChanged_ = true;
//}


//void EditPVDialog::colorEntered(const QString &newColor)
//{
//    color_ = newColor;

//    if (color_ == "")
//        colorChanged_ = false;
//    else
//        colorChanged_ = true;
//}


//void EditPVDialog::displayMaxEntered(const QString &max)
//{
//    displayMax_ = max;
//    qDebug() << "EditPVDialog :: new display max entered :" << max << ".";
//    yMaxChanged_ = true;

//}


//void EditPVDialog::displayMinEntered(const QString &min)
//{
//    displayMin_ = min;

//    qDebug() << "EditPVDialog :: new display min entered :" << min;
//    yMinChanged_ = true;
//}



////void EditPVDialog::shiftAmountEntered(const QString &shift)
////{
////    shiftAmount_ = shift;

////    if (shiftAmount_ == "")
////        shiftAmountChanged_ = false;
////    else
////        shiftAmountChanged_ = true;
////}



//void EditPVDialog::toUpdateDataRange(MPlotAxisRange *newRange)
//{
//    qreal min = newRange->min();
//    qreal max = newRange->max();

//    if (min < max) {
//        currentDisplayMax_->setText(QString::number(max, 'f', 2));
//        currentDisplayMin_->setText(QString::number(min, 'f', 2));

//    } else {
//        currentDisplayMax_->setText(QString::number(min, 'f', 2));
//        currentDisplayMin_->setText(QString::number(max, 'f', 2));
//    }
//}




//void EditPVDialog::automaticDisplayMaxChanged(Qt::CheckState checkState)
//{
//    if (checkState == Qt::Checked)
//        displayMax_ = "";
//    else
//        displayMax_ = displayMaxEntry_->text();
//}



//void EditPVDialog::automaticDisplayMinChanged(Qt::CheckState checkState)
//{
//    if (checkState == Qt::Checked)
//        displayMin_ = "";
//    else
//        displayMin_ = displayMinEntry_->text();
//}



//void EditPVDialog::setMaxCheckState(const QString &textEntered)
//{
//    if (textEntered == "")
//        automaticMax_->setCheckState(Qt::Checked);
//    else
//        automaticMax_->setCheckState(Qt::Unchecked);
//}



//void EditPVDialog::setMinCheckState(const QString &textEntered)
//{
//    if (textEntered == "")
//        automaticMin_->setCheckState(Qt::Checked);
//    else
//        automaticMin_->setCheckState(Qt::Unchecked);
//}


//void EditPVDialog::toClearMaxEntry(int checkState)
//{
//    if (checkState == Qt::Checked)
//        displayMaxEntry_->setText("");
//}



//void EditPVDialog::toClearMinEntry(int checkState)
//{
//    if (checkState == Qt::Checked)
//        displayMinEntry_->setText("");
//}
