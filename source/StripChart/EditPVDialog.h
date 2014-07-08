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


#ifndef EDITPVDIALOG_H
#define EDITPVDIALOG_H

#include <QDialog>
#include <QtGui>

#include "MPlot/MPlotAxisScale.h"

#include "StripChart/StripToolBasicVariableInfo.h"

/// This is a dialog that pops up when the user indicates they want to edit a single/group of pvs. The pvs to edit are displayed, then space is provided for new description, points, unit to be entered. The entered values are applied to all selected pvs.

class EditPVDialog : public QDialog
{
    Q_OBJECT

public:
//<<<<<<< HEAD
//    EditPVDialog(QStringList pvNames, QWidget *parent = 0);
//    virtual ~EditPVDialog();
//    friend class StripToolModel;
    
//signals:
    
//=======
    EditPVDialog(QWidget *parent = 0);

signals:
    void dataMaxUpdated(double newMax);
    void dataMinUpdated(double newMin);

public:
    void displayName(const QString &infoName, bool editEnabled);

    void displayAutoImport(const QString autoImportEnabled, bool editEnabled);
    bool importStateChanged() const;
    QString importState() const;

    void displayCreationDateTime(const QString &infoCreation, bool editEnabled);

    void displayDescription(const QString &infoDescription, bool editEnabled);
    bool descriptionChanged() const;
    QString description() const;

    void displayUnits(const QString &infoUnits, bool editEnabled);
    bool unitsChanged() const;
    QString units() const;

    void displayGranularity(const QString &infoGranularity, bool editEnabled);
    bool granularityChanged() const;
    QString granularity() const;

    void displayAverageOverPoints(const QString &infoAverage, bool editEnabled);
    bool averageOverPointsChanged() const;
    QString averageOverPoints() const;

    void displayColorName(const QString &infoColorName, bool editEnabled);
    bool colorNameChanged() const;
    QString colorName() const;

    void displayAxisMax(const QString &axisMax, bool editEnabled);
    bool axisMaxChanged() const;
    QString axisMax() const;

    void displayAxisMin(const QString &axisMin, bool editEnabled);
    bool axisMinChanged() const;
    QString axisMin() const;

protected slots:
    void autoImportStateChanged(const int state);
    void descriptionEntered(const QString &newDescription);
    void unitsEntered(const QString &newUnits);
    void granularityEntered(const QString &newGran);
    void averageOverPointsEntered(const QString &newAverage);
    void colorNameEntered(const QString &newColor);
    void axisMaxEntered(const QString &newMax);
    void axisMinEntered(const QString &newMin);


//    void displayMaxEntered(const QString &max);
//    void displayMinEntered(const QString &min);

//    void automaticDisplayMaxChanged(Qt::CheckState checkState);
//    void automaticDisplayMinChanged(Qt::CheckState checkState);
//    void setMaxCheckState(const QString &textEntered);
//    void setMinCheckState(const QString &textEntered);
//    void toClearMaxEntry(int checkState);
//    void toClearMinEntry(int checkState);

//    void toUpdateDataRange(MPlotAxisRange *newRange);

//>>>>>>> master
private:
//    QLayout* createFormGui();
    QLayout* createButtonGui();
    void defaultSettings();

private:
    int componentCount_;
    QGridLayout *componentLayout_;

    QString importState_;
    bool importStateChanged_;

    QString description_;
    bool descriptionChanged_;

    QString units_;
    bool unitsChanged_;

    QString granularity_;
    bool granularityChanged_;

    QString averageOverPoints_;
    bool averageOverPointsChanged_;

    QString colorName_;
    bool colorNameChanged_;

    QString axisMax_;
    bool axisMaxChanged_;

    QString axisMin_;
    bool axisMinChanged_;

//    QString gran_;
//    bool granChanged_;
//    QLineEdit *granularityEntry_;

//    QString color_;
//    bool colorChanged_;
//    QLineEdit *colorEntry_;

//    QString displayMax_;
//    bool yMaxChanged_;
//    QLineEdit *displayMaxEntry_;
//    QLabel *currentDisplayMax_;
//    QCheckBox *automaticMax_;

//    QString displayMin_;
//    bool yMinChanged_;
//    QLineEdit *displayMinEntry_;
//    QLabel *currentDisplayMin_;
//    QCheckBox *automaticMin_;

    QPushButton *okButton_;
    QPushButton *cancelButton_;
    QPushButton *restoreButton_;
    
};

#endif // EDITPVDIALOG_H
