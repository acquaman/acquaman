#ifndef EDITPVDIALOG_H
#define EDITPVDIALOG_H

#include <QDialog>
#include <QtGui>

/// This is a dialog that pops up when the user indicates they want to edit a single/group of pvs. The pvs to edit are displayed, then space is provided for new description, points, unit to be entered. The entered values are applied to all selected pvs.

class EditPVDialog : public QDialog
{
    Q_OBJECT

public:
    EditPVDialog(QStringList pvNames, QStringList pvDefaults, QWidget *parent = 0);
    ~EditPVDialog();
    friend class StripToolModel;
    
signals:
    void restoreSelectedDefaults();
    
private:
    QStringList defaults_;
    QString* description_;
    QString* units_;
    int* gran_;
    QString* color_;
    double* displayMax_;
    double* displayMin_;

    QLineEdit *descriptionEntry_;
    QLineEdit *unitsEntry_;
    QLineEdit *granularityEntry_;
    QLineEdit *colorEntry_;
    QLineEdit *displayMaxEntry_;
    QLineEdit *displayMinEntry_;

    QPushButton *okButton_;
    QPushButton *cancelButton_;
    QPushButton *restoreButton_;

protected:
    /// Returns the description entered by the user, an empty string if nothing entered.
    QString* description();
    /// Returns the units entered by the user, an empty string if nothing entered.
    QString* units();
    int* granularity();
    QString* color();
    double* displayMax();
    double* displayMin();

protected slots:
    /// When the user finishes editing the text in descriptionEntry_, the new text is saved to description_.
    void descriptionEntered(QString &newDescription);
    /// When the user finishes editing the text in unitsEntry_, the new text is saved to units_.
    void unitsEntered(QString &units);
    void granularityEntered(QString &gran);
    void colorEntered(QString &newColor);
    void displayMaxEntered(QString &max);
    void displayMinEntered(QString &min);

    void toRestoreMaxDisplay();
    void toRestoreMinDisplay();
    void restore();
    
};

#endif // EDITPVDIALOG_H
