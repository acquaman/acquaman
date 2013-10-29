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
    /// Enables/disables information entry.
    void onControlsEnabled(bool);
    /// Clears the pv valid feedback information.
    void clearMessage();
    /// If the pv name entered is not valid, an error message is displayed. No message if valid.
    void displayMessage(bool isValid);
    /// When the add button is clicked, information entry is disabled while we test to see if the entered pv name is valid.
    void addClicked();
    /// Attempts to connect to the pv with the entered name.
    void testValidity(const QString &pvName);
    /// If the connection is successful, add the pv to the model.
    void onPVConnected(bool isConnected);
    /// If the connection is unsuccessful, cause a message to be displayed.
    void onPVError(int errorNum);
};

#endif // STRIPTOOLQUICKCONTROLS_H
