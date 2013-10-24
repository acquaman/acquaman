#ifndef STRIPTOOLQUICKCONTROLS_H
#define STRIPTOOLQUICKCONTROLS_H

#include <QWidget>
#include <QtGui>

#include "beamline/AMPVControl.h"
#include "StripChart/StripToolModel.h"
#include "StripChart/StripToolListView.h"

class StripToolQuickControls : public QWidget
{
    Q_OBJECT

public:
    explicit StripToolQuickControls(QWidget *parent = 0, StripToolModel *newModel = 0);
    
signals:
    void controlsEnabled(bool);
    void addPV(const QString &name, const QString &description, const QString &units);
    void isValid(bool isValid);

private:
    StripToolModel *model_;
    StripToolListView *listView_;
    QString pvName_;
    AMReadOnlyPVControl *pvControl_;
    QLineEdit *pvNameLineEdit_;
    QPushButton *addButton_;
    QLabel *message_;
    
protected slots:
    void onControlsEnabled(bool);
    void clearMessage();
    void displayMessage(bool isValid);
    void addClicked();
    void testValidity(const QString &pvName);
    void onPVConnected(bool isConnected);
    void onPVError(int errorNum);
};

#endif // STRIPTOOLQUICKCONTROLS_H
