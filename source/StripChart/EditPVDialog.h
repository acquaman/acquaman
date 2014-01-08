#ifndef EDITPVDIALOG_H
#define EDITPVDIALOG_H

#include <QDialog>
#include <QtGui>

/// This is a dialog that pops up when the user indicates they want to edit a single/group of pvs. The pvs to edit are displayed, then space is provided for new description, points, unit to be entered. The entered values are applied to all selected pvs.

class EditPVDialog : public QDialog
{
    Q_OBJECT

public:
    EditPVDialog(QStringList pvNames, QWidget *parent = 0);
    ~EditPVDialog();
    friend class StripToolModel;
    
signals:
    void restoreSelectedDefaults();

//public:
//    void updateCurrentDisplayMax(double newMax);
//    void updateCurrentDisplayMin(double newMin);

private:
    QString description_;
    bool descriptionChanged_;
    QLineEdit *descriptionEntry_;

    QString units_;
    bool unitsChanged_;
    QLineEdit *unitsEntry_;

    int gran_;
    bool granChanged_;
    QLineEdit *granularityEntry_;

    QString color_;
    bool colorChanged_;
    QLineEdit *colorEntry_;

    QString displayMax_;
    bool yMaxChanged_;
    QLineEdit *displayMaxEntry_;
    QLabel *currentDisplayMax_;

    QString displayMin_;
    bool yMinChanged_;
    QLineEdit *displayMinEntry_;
    QLabel *currentDisplayMin_;

    QPushButton *okButton_;
    QPushButton *cancelButton_;
    QPushButton *restoreButton_;

protected:
    /// Returns the description entered by the user, an empty string if nothing entered.
    QString description();
    bool descriptionChanged();

    /// Returns the units entered by the user, an empty string if nothing entered.
    QString units();
    bool unitsChanged();

    int granularity();
    bool granularityChanged();

    QString color();
    bool colorChanged();

    QString displayMax();
    bool displayMaxChanged();

    QString displayMin();
    bool displayMinChanged();

protected slots:
    /// When the user finishes editing the text in descriptionEntry_, the new text is saved to description_.
    void descriptionEntered(const QString &newDescription);

    /// When the user finishes editing the text in unitsEntry_, the new text is saved to units_.
    void unitsEntered(const QString &units);
    void granularityEntered(const QString &gran);
    void colorEntered(const QString &newColor);
    void displayMaxEntered(const QString &max);
    void displayMinEntered(const QString &min);

    void toUpdateCurrentDisplayMax(double currentMax);
    void toUpdateCurrentDisplayMin(double currentMin);
    
};

#endif // EDITPVDIALOG_H
