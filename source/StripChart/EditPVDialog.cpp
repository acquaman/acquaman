#include "EditPVDialog.h"

EditPVDialog::EditPVDialog(QWidget *parent) :
    QDialog(parent)
{
    componentCount_ = 0;
    componentLayout_ = new QGridLayout();

    descriptionChanged_ = false;
    unitsChanged_ = false;

    QVBoxLayout *dialogLayout = new QVBoxLayout();
    dialogLayout->addLayout(componentLayout_);
    dialogLayout->addLayout(createButtonGui());

    setLayout(dialogLayout);
    setWindowTitle("Edit Info");
    setModal(true);
}



void EditPVDialog::displayName(const QString &infoName, bool editEnabled) {
    QLabel *nameLabel = new QLabel("Name : ");
    QLineEdit *nameEntry = new QLineEdit(infoName);
    nameEntry->setEnabled(editEnabled);
    componentLayout_->addWidget(nameLabel, componentCount_, 0);
    componentLayout_->addWidget(nameEntry, componentCount_, 1);

    componentCount_++;
}



void EditPVDialog::displayCreationDateTime(const QString &infoCreation, bool editEnabled) {
    QLabel *creationLabel = new QLabel("Date added : ");
    QLineEdit *creationEntry = new QLineEdit(infoCreation);
    creationEntry->setEnabled(editEnabled);
    componentLayout_->addWidget(creationLabel, componentCount_, 0);
    componentLayout_->addWidget(creationEntry, componentCount_, 1);

    componentCount_++;
}



void EditPVDialog::displayDescription(const QString &infoDescription, bool editEnabled) {
    QLabel *descriptionLabel = new QLabel("Description : ");
    QLineEdit *descriptionEntry = new QLineEdit(infoDescription);
    descriptionEntry->setEnabled(editEnabled);
    connect( descriptionEntry, SIGNAL(textChanged(QString)), this, SLOT(descriptionEntered(QString)) );

    componentLayout_->addWidget(descriptionLabel, componentCount_, 0);
    componentLayout_->addWidget(descriptionEntry, componentCount_, 1);

    componentCount_++;
}




bool EditPVDialog::descriptionChanged() const {
    return descriptionChanged_;
}



QString EditPVDialog::description() const {
    return description_;
}



void EditPVDialog::displayUnits(const QString &infoUnits, bool editEnabled) {
    QLabel *unitsLabel = new QLabel("Units : ");
    QLineEdit *unitsEntry = new QLineEdit(infoUnits);
    unitsEntry->setEnabled(editEnabled);
    connect( unitsEntry, SIGNAL(textChanged(QString)), this, SLOT(unitsEntered(QString)) );
    componentLayout_->addWidget(unitsLabel, componentCount_, 0);
    componentLayout_->addWidget(unitsEntry, componentCount_, 1);

    componentCount_++;
}



bool EditPVDialog::unitsChanged() const {
    return unitsChanged_;
}



QString EditPVDialog::units() const {
    return units_;
}



void EditPVDialog::displayGranularity(const QString &infoGranularity, bool editEnabled) {
    QLabel *granLabel = new QLabel("Granularity : ");
    QLineEdit *granEntry = new QLineEdit(infoGranularity);
    granEntry->setEnabled(editEnabled);
    connect( granEntry, SIGNAL(textChanged(QString)), this, SLOT(granularityEntered(QString)) );
    componentLayout_->addWidget(granLabel, componentCount_, 0);
    componentLayout_->addWidget(granEntry, componentCount_, 1);

    componentCount_++;
}



bool EditPVDialog::granularityChanged() const {
    return granularityChanged_;
}



QString EditPVDialog::granularity() const {
    return granularity_;
}



void EditPVDialog::displayColorName(const QString &infoColorName, bool editEnabled) {
    QLabel *colorLabel = new QLabel("Color : ");
    QLineEdit *colorEntry = new QLineEdit(infoColorName);
    colorEntry->setEnabled(editEnabled);
    connect( colorEntry, SIGNAL(textChanged(QString)), this, SLOT(colorNameEntered(QString)) );
    componentLayout_->addWidget(colorLabel, componentCount_, 0);
    componentLayout_->addWidget(colorEntry, componentCount_, 1);

    componentCount_++;
}



bool EditPVDialog::colorNameChanged() const {
    return colorNameChanged_;
}



QString EditPVDialog::colorName() const {
    return colorName_;
}



void EditPVDialog::descriptionEntered(const QString &newDescription)
{
    description_ = newDescription;
    descriptionChanged_ = true;
}



void EditPVDialog::unitsEntered(const QString &newUnits) {
    units_ = newUnits;
    unitsChanged_ = true;
}



void EditPVDialog::granularityEntered(const QString &newGran) {
    granularity_ = newGran;
    granularityChanged_ = true;
}



void EditPVDialog::colorNameEntered(const QString &newColor) {
    colorName_ = newColor;
    colorNameChanged_ = true;
}



//QLayout* EditPVDialog::createFormGui()
//{

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



QLayout* EditPVDialog::createButtonGui() {
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    okButton_ = new QPushButton("Ok");
    okButton_->setDefault(true);
    connect( okButton_, SIGNAL(clicked()), this, SLOT(accept()) );
    buttonLayout->addWidget(okButton_);

    cancelButton_ = new QPushButton("Cancel");
    connect( cancelButton_, SIGNAL(clicked()), this, SLOT(reject()) );
    buttonLayout->addWidget(cancelButton_);

    return buttonLayout;
}


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
