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
    
private:
    QString description_;
    QString units_;
    int points_;
    QListView *nameList_;
    QLineEdit *descriptionEntry_;
    QLineEdit *unitsEntry_;
    QLineEdit *pointsEntry_;
    QPushButton *okButton_;

protected:
    /// Returns the description entered by the user, an empty string if nothing entered.
    QString description();
    /// Returns the units entered by the user, an empty string if nothing entered.
    QString units();
    /// Returns the number of points the user wants to be displayed, 0 if nothing entered (and ignored later).
    int points();

protected slots:
    /// When the user finishes editing the text in descriptionEntry_, the new text is saved to description_.
    void descriptionEntered(const QString &newDescription);
    /// When the user finishes editing the text in unitsEntry_, the new text is saved to units_.
    void unitsEntered(const QString &units);
    /// When the user finishes editing the text in pointsEntry_, the string is converted to an int and is saved to points_.
    void pointsEntered(const QString &points);
    
};

#endif // EDITPVDIALOG_H
