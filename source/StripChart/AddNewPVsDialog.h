#ifndef ADDNEWPVSDIALOG_H
#define ADDNEWPVSDIALOG_H

#include <QDialog>
#include <QtGui>

#include "StripChart/AddPVFromInfoTab.h"

class AddNewPVsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddNewPVsDialog(QWidget *parent = 0);
    ~AddNewPVsDialog();
    friend class StripToolView;
    
signals:

protected:
    AddPVFromInfoTab *pvFromInfoTab_;
    QTabWidget *tabWidget_;
    QDialogButtonBox *buttonBox_;

protected:
    QString pvName();
    QString pvDescription();
    QString pvUnits();
    
};

#endif // ADDNEWPVSDIALOG_H
