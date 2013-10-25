#ifndef EDITPVDIALOG_H
#define EDITPVDIALOG_H

#include <QDialog>
#include <QtGui>

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
    QString description();
    QString units();
    int points();

protected slots:
    void descriptionEntered(const QString &newDescription);
    void unitsEntered(const QString &units);
    void pointsEntered(const QString &points);
    
};

#endif // EDITPVDIALOG_H
