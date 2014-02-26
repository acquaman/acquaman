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

    friend class StripToolModel;

public:
    EditPVDialog(StripToolBasicVariableInfo *info, QWidget *parent = 0);
//    ~EditPVDialog();

//protected:
//    /// Returns the description entered by the user, an empty string if nothing entered.
//    QString description();
//    bool descriptionChanged();

//    /// Returns the units entered by the user, an empty string if nothing entered.
//    QString units();
//    bool unitsChanged();

//    QString granularity();
//    bool granularityChanged();

//    QString color();
//    bool colorChanged();

//    QString displayMax();
//    bool displayMaxChanged();

//    QString displayMin();
//    bool displayMinChanged();

//private slots:
//    /// When the user finishes editing the text in descriptionEntry_, the new text is saved to description_.
//    void descriptionEntered(const QString &newDescription);

//    /// When the user finishes editing the text in unitsEntry_, the new text is saved to units_.
//    void unitsEntered(const QString &units);
//    void granularityEntered(const QString &gran);
//    void colorEntered(const QString &newColor);
//    void displayMaxEntered(const QString &max);
//    void displayMinEntered(const QString &min);

//    void automaticDisplayMaxChanged(Qt::CheckState checkState);
//    void automaticDisplayMinChanged(Qt::CheckState checkState);
//    void setMaxCheckState(const QString &textEntered);
//    void setMinCheckState(const QString &textEntered);
//    void toClearMaxEntry(int checkState);
//    void toClearMinEntry(int checkState);

//    void toUpdateDataRange(MPlotAxisRange *newRange);

//private:
//    QLayout* createFormGui();
//    QLayout* createButtonGui();

//private:
//    StripToolVariableInfo* info_;

//    QString description_;
//    bool descriptionChanged_;
//    QLineEdit *descriptionEntry_;

//    QString units_;
//    bool unitsChanged_;
//    QLineEdit *unitsEntry_;

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

//    QPushButton *okButton_;
//    QPushButton *cancelButton_;
//    QPushButton *restoreButton_;
    
};

#endif // EDITPVDIALOG_H
