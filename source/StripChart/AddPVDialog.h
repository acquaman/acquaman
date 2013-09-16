#ifndef ADDPVDIALOG_H
#define ADDPVDIALOG_H

#include <QWidget>
#include <QtGui>

#include "beamline/AMPVControl.h"

//class AddPVDialog : public QWidget
class AddPVDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddPVDialog(QWidget *parent = 0);

signals:
    void closeDialog();

protected:
//    int pvValidValue;

    QDialog *addPVDialog_;
    QLabel *pvNamePrompt_;
    QLineEdit *pvNameLineEdit_;
    QLabel *pvValidLabel_;

    QString pvNameEntered_;
    QString newActivePVName_;

    QPushButton *acceptButton_;
    QPushButton *cancelButton_;

    AMReadOnlyPVControl *pvControl_;


protected slots:
    void onPVConnected(bool);
    void onAcceptButtonClicked();
    void onCancelButtonClicked();
    void enableAcceptButton();
    void onCloseDialogEmitted();
    
};

#endif // ADDPVDIALOG_H
