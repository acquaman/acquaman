#ifndef STRIPTOOLQUICKCONTROLS_H
#define STRIPTOOLQUICKCONTROLS_H

#include <QWidget>
#include <QtGui>

#include "beamline/AMPVControl.h"
#include "StripChart/StripToolModel.h"
#include "StripChart/StripToolListView.h"

/// This class is a sort of optional sidebar that provides a number of ways for the user to interact with an added pv, as well as a quick way to add a new one.
class StripToolQuickControls : public QWidget
{
    Q_OBJECT

public:
    explicit StripToolQuickControls(QWidget *parent = 0);
    
signals:
    void reset();
    void clearName();
    void clearMessage();
    void nameEntryEnabled(bool);
    void nameEntryFocus();
    void buttonEnabled(bool);
    void pvConnected(bool isConnected);
    void addPV(const QString &pvName);
    void error(const QString &errorMessage);
//    void testComplete(bool isValid, AMControl *pvControl);

protected:
    StripToolModel *model_;
    QTimer *timer_;
    StripToolListView *listView_;
    QLineEdit *pvNameLineEdit_;
    QPushButton *addButton_;
    QLabel *message_;

public:
    void setModel(StripToolModel *newModel);

protected:
    /// Attempts to connect to the pv with the entered name.
    void testValidity(const QString &pvName);
    
protected slots:
    void displayMessage(const QString &text);
    /// When the add button is clicked, information entry is disabled while we test to see if the entered pv name is valid.
    void addClicked();
    void resetControls();

//    /// If the connection is successful, add the pv to the model.
//    void onPVConnected(bool isConnected);

//    void onTestComplete(bool isValid, AMControl *pvControl);

};

#endif // STRIPTOOLQUICKCONTROLS_H
