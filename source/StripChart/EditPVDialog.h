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
    EditPVDialog(QWidget *parent = 0);

public:
    void displayName(const QString &infoName, bool editEnabled);

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

    void displayColorName(const QString &infoColorName, bool editEnabled);
    bool colorNameChanged() const;
    QString colorName() const;


//    QString displayMax();
//    bool displayMaxChanged();

//    QString displayMin();
//    bool displayMinChanged();

public slots:
    void descriptionEntered(const QString &newDescription);
    void unitsEntered(const QString &newUnits);
    void granularityEntered(const QString &newGran);
    void colorNameEntered(const QString &newColor);

//    void displayMaxEntered(const QString &max);
//    void displayMinEntered(const QString &min);

//    void automaticDisplayMaxChanged(Qt::CheckState checkState);
//    void automaticDisplayMinChanged(Qt::CheckState checkState);
//    void setMaxCheckState(const QString &textEntered);
//    void setMinCheckState(const QString &textEntered);
//    void toClearMaxEntry(int checkState);
//    void toClearMinEntry(int checkState);

//    void toUpdateDataRange(MPlotAxisRange *newRange);

private:
//    QLayout* createFormGui();
    QLayout* createButtonGui();

private:
//    StripToolVariableInfo* info_;
    int componentCount_;
    QGridLayout *componentLayout_;

    QString description_;
    bool descriptionChanged_;

    QString units_;
    bool unitsChanged_;

    QString granularity_;
    bool granularityChanged_;

    QString colorName_;
    bool colorNameChanged_;

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
