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
    friend class StripTool;

signals:
    void enableButtonBox(bool isEnabled);
    void enablePVName(bool isEnabled);
    void enablePVDescription(bool isEnabled);
    void enablePVUnits(bool isEnabled);
    void newPVIsValid();

protected:
    QString pvName_;
    QString pvDescription_;
    QString pvUnits_;
    QLabel *pvValidMessage_;

protected:
    QString pvName();
    QString pvDescription();
    QString pvUnits();

protected slots:
    void onPVNameEntered(const QString name);
    void onPVDescriptionEntered(const QString description);
    void onPVUnitsEntered(const QString units);
    void onAcceptButtonClicked();
    void pvIsValid(bool isValid);
};

#endif // ADDPVDIALOG_H
