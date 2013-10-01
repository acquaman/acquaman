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
    void enableButtonBox(bool isEnabled);
    void enablePVName(bool isEnabled);
    void enablePVDescription(bool isEnabled);
    void newPVIsValid();
    void newPVAccepted(const QString &pvName, const QString &pvDescription);

protected:

    QString pvName_;
    QString pvDescription_;
    QLabel *pvValidMessage_;

protected slots:
    void onPVNameEntered(const QString name);
    void onPVDescriptionEntered(const QString description);
    void onPVConnected(bool isConnected);
    void onAcceptButtonClicked();
    void onNewPVIsValid();
    
};

#endif // ADDPVDIALOG_H
