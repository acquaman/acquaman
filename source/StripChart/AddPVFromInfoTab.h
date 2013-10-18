#ifndef ADDPVFROMINFOTAB_H
#define ADDPVFROMINFOTAB_H

#include <QWidget>
#include <QtGui>


#include "beamline/AMPVControl.h"

class AddPVFromInfoTab : public QWidget
{
    Q_OBJECT

public:
    explicit AddPVFromInfoTab(QWidget *parent = 0);
    ~AddPVFromInfoTab();
    friend class AddNewPVsDialog;

signals:
    void enablePVName(bool isEnabled);
    void enablePVDescription(bool isEnabled);
    void enablePVUnits(bool isEnabled);
    void enablePVEntry(bool isEnabled);
    void enableButtonBox(bool isEnabled);

protected:
    QString pvName_;
    QString pvDescription_;
    QString pvUnits_;
    QLabel *pvValidMessage_;

protected:
    QString pvName();
    QString pvDescription();
    QString pvUnits();
    void checkValidity(const QString &pvName);

protected slots:
    void onPVNameEntered(const QString &name);
    void onPVDescriptionEntered(const QString &description);
    void onPVUnitsEntered(const QString &units);
    void setPVEntryEnabled(bool isEnabled);
    void pvIsValid(bool isValid);
};

#endif // ADDPVFROMINFOTAB_H
