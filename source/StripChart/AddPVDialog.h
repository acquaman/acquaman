#ifndef ADDPVDIALOG_H
#define ADDPVDIALOG_H

#include <QWidget>
#include <QtGui>

#include "beamline/AMPVControl.h"

class AddPVDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddPVDialog(QWidget *parent = 0);

signals:
    void enableAcceptButton(bool isEnabled);
    void enableCancelButton(bool isEnabled);
    void enablePVName(bool isEnabled);
    void enablePVDescription(bool isEnabled);
    void newPVIsValid();
    void newPVAccepted(const QString &pvName, const QString &pvDescription);

protected:

    QString acceptedPVName_;
    QString acceptedPVDescription_;

    QLabel *pvNamePrompt_;
    QLineEdit *pvNameLineEdit_;
    QString pvNameEntered_;

    QLabel *pvDescriptionPrompt_;
    QLineEdit *pvDescriptionLineEdit_;
    QString pvDescriptionEntered_;

    QLabel *pvValidLabel_;

    QPushButton *acceptButton_;
    QPushButton *cancelButton_;

    AMReadOnlyPVControl *pvControl_;

protected slots:
    void onPVNameEntered();
    void onPVConnected(bool isConnected);
    void onAcceptButtonClicked();
    void onCancelButtonClicked();
    void onNewPVIsValid();
    
};

#endif // ADDPVDIALOG_H
