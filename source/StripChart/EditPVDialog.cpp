/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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



void EditPVDialog::displayName(const QString &infoName, bool editEnabled)
{
    QLabel *nameLabel = new QLabel("Name : ");
    QLineEdit *nameEntry = new QLineEdit(infoName);
    nameEntry->setEnabled(editEnabled);
    componentLayout_->addWidget(nameLabel, componentCount_, 0);
    componentLayout_->addWidget(nameEntry, componentCount_, 1);

    componentCount_++;
}



void EditPVDialog::displayAutoImport(const QString autoImportEnabled, bool editEnabled)
{
    QCheckBox *importState = new QCheckBox("Auto import");
    if (autoImportEnabled == "true")
        importState->setCheckState(Qt::Checked);
    else
        importState->setCheckState(Qt::Unchecked);
    importState->setEnabled(editEnabled);
    connect( importState, SIGNAL(stateChanged(int)), this, SLOT(autoImportStateChanged(int)) );
    componentLayout_->addWidget(importState, componentCount_, 1);

    componentCount_++;
}



bool EditPVDialog::importStateChanged() const
{
    return importStateChanged_;
}



QString EditPVDialog::importState() const
{
    return importState_;
}



void EditPVDialog::displayCreationDateTime(const QString &infoCreation, bool editEnabled)
{
    QLabel *creationLabel = new QLabel("Date added : ");
    QLineEdit *creationEntry = new QLineEdit(infoCreation);
    creationEntry->setEnabled(editEnabled);
    componentLayout_->addWidget(creationLabel, componentCount_, 0);
    componentLayout_->addWidget(creationEntry, componentCount_, 1);

    componentCount_++;
}



void EditPVDialog::displayDescription(const QString &infoDescription, bool editEnabled)
{
    QLabel *descriptionLabel = new QLabel("Description : ");
    QLineEdit *descriptionEntry = new QLineEdit(infoDescription);
    descriptionEntry->setEnabled(editEnabled);
    connect( descriptionEntry, SIGNAL(textChanged(QString)), this, SLOT(descriptionEntered(QString)) );

    componentLayout_->addWidget(descriptionLabel, componentCount_, 0);
    componentLayout_->addWidget(descriptionEntry, componentCount_, 1);

    componentCount_++;
}




bool EditPVDialog::descriptionChanged() const
{
    return descriptionChanged_;
}



QString EditPVDialog::description() const
{
    return description_;
}



void EditPVDialog::displayUnits(const QString &infoUnits, bool editEnabled)
{
    QLabel *unitsLabel = new QLabel("Units : ");
    QLineEdit *unitsEntry = new QLineEdit(infoUnits);
    unitsEntry->setEnabled(editEnabled);
    connect( unitsEntry, SIGNAL(textChanged(QString)), this, SLOT(unitsEntered(QString)) );
    componentLayout_->addWidget(unitsLabel, componentCount_, 0);
    componentLayout_->addWidget(unitsEntry, componentCount_, 1);

    componentCount_++;
}



bool EditPVDialog::unitsChanged() const
{
    return unitsChanged_;
}



QString EditPVDialog::units() const
{
    return units_;
}



void EditPVDialog::displayGranularity(const QString &infoGranularity, bool editEnabled)
{
    QLabel *granLabel = new QLabel("Granularity : ");
    QLineEdit *granEntry = new QLineEdit(infoGranularity);
    granEntry->setEnabled(editEnabled);
    connect( granEntry, SIGNAL(textChanged(QString)), this, SLOT(granularityEntered(QString)) );
    componentLayout_->addWidget(granLabel, componentCount_, 0);
    componentLayout_->addWidget(granEntry, componentCount_, 1);

    componentCount_++;
}



bool EditPVDialog::granularityChanged() const
{
    return granularityChanged_;
}



QString EditPVDialog::granularity() const
{
    return granularity_;
}



void EditPVDialog::displayAverageOverPoints(const QString &infoAverage, bool editEnabled)
{
    QLabel *averageLabel = new QLabel("Average : ");
    QLineEdit *averageEntry = new QLineEdit(infoAverage);
    averageEntry->setEnabled(editEnabled);
    connect( averageEntry, SIGNAL(textChanged(QString)), this, SLOT(averageOverPointsEntered(QString)) );
    componentLayout_->addWidget(averageLabel, componentCount_, 0);
    componentLayout_->addWidget(averageEntry, componentCount_, 1);

    componentCount_++;
}



bool EditPVDialog::averageOverPointsChanged() const
{
    return averageOverPointsChanged_;
}



QString EditPVDialog::averageOverPoints() const
{
    return averageOverPoints_;
}



void EditPVDialog::displayColorName(const QString &infoColorName, bool editEnabled)
{
    QLabel *colorLabel = new QLabel("Color : ");
    QLineEdit *colorEntry = new QLineEdit(infoColorName);
    colorEntry->setEnabled(editEnabled);
    connect( colorEntry, SIGNAL(textChanged(QString)), this, SLOT(colorNameEntered(QString)) );
    componentLayout_->addWidget(colorLabel, componentCount_, 0);
    componentLayout_->addWidget(colorEntry, componentCount_, 1);

    componentCount_++;
}



bool EditPVDialog::colorNameChanged() const
{
    return colorNameChanged_;
}



QString EditPVDialog::colorName() const
{
    return colorName_;
}



void EditPVDialog::displayAxisMax(const QString &axisMax, bool editEnabled)
{
    QLabel *maxLabel = new QLabel("Axis max : ");

    QLineEdit *maxEntry = new QLineEdit(axisMax);
    maxEntry->setEnabled(editEnabled);
    connect( maxEntry, SIGNAL(textChanged(QString)), this, SLOT(axisMaxEntered(QString)) );

    QLabel *currentMax = new QLabel("---");
    connect( this, SIGNAL(dataMaxUpdated(double)), currentMax, SLOT(setNum(double)) );

    componentLayout_->addWidget(maxLabel, componentCount_, 0);
    componentLayout_->addWidget(maxEntry, componentCount_, 1);
    componentLayout_->addWidget(currentMax, componentCount_, 2);

    componentCount_++;
}



bool EditPVDialog::axisMaxChanged() const
{
    return axisMaxChanged_;
}



QString EditPVDialog::axisMax() const
{
    return axisMax_;
}



void EditPVDialog::displayAxisMin(const QString &axisMin, bool editEnabled)
{
    QLabel *minLabel = new QLabel("Axis min : ");

    QLineEdit *minEntry = new QLineEdit(axisMin);
    minEntry->setEnabled(editEnabled);
    connect( minEntry, SIGNAL(textChanged(QString)), this, SLOT(axisMinEntered(QString)) );

    QLabel *currentMin = new QLabel("---");
    connect( this, SIGNAL(dataMinUpdated(double)), currentMin, SLOT(setNum(double)) );

    componentLayout_->addWidget(minLabel, componentCount_, 0);
    componentLayout_->addWidget(minEntry, componentCount_, 1);
    componentLayout_->addWidget(currentMin, componentCount_, 2);

    componentCount_++;
}



bool EditPVDialog::axisMinChanged() const
{
    return axisMinChanged_;
}



QString EditPVDialog::axisMin() const
{
    return axisMin_;
}



void EditPVDialog::autoImportStateChanged(const int state)
{
    if (state == Qt::Checked) {
        importStateChanged_ = true;
        importState_ = "true";
    } else {
        importStateChanged_ = true;
        importState_ = "false";
    }
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



void EditPVDialog::granularityEntered(const QString &newGran)
{
    granularity_ = newGran;
    granularityChanged_ = true;
}



void EditPVDialog::averageOverPointsEntered(const QString &newAverage)
{
    averageOverPoints_ = newAverage;
    averageOverPointsChanged_ = true;
}



void EditPVDialog::colorNameEntered(const QString &newColor)
{
    colorName_ = newColor;
    colorNameChanged_ = true;
}



void EditPVDialog::axisMaxEntered(const QString &newMax)
{
    axisMax_ = newMax;
    axisMaxChanged_ = true;
}



void EditPVDialog::axisMinEntered(const QString &newMin)
{
    axisMin_ = newMin;
    axisMinChanged_ = true;
}



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



void EditPVDialog::defaultSettings()
{
    importStateChanged_ = false;
    descriptionChanged_ = false;
    unitsChanged_ = false;
    granularityChanged_ = false;
    averageOverPointsChanged_ = false;
    colorNameChanged_ = false;
    axisMaxChanged_ = false;
    axisMinChanged_ = false;
}
