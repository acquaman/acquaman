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
    ~StripToolQuickControls();
    
signals:
    void reset();
    void clearName();
    void clearMessage();
    void nameEntryEnabled(bool);
    void nameEntryFocus();
    void buttonEnabled(bool);
    void addPV(const QString &pvName);
    void error(const QString &errorMessage);
    void pausePVs();
    void resumePVs();
    void updateTime(int newTime);
    void updateUnits(const QString &newUnits);

protected:
    StripToolModel *model_;
    StripToolListView *listView_;
    QPushButton *pauseButton_;
    QPushButton *resumeButton_;
    QComboBox *timeUnits_;
    QLabel *message_;

public:
    void setModel(StripToolModel *newModel);
    QString getCurrentUnits() const;
    
protected slots:
    void resetControls();
    void pauseClicked();
    void resumeClicked();
    void timeValueChanged(int newTime);
    void timeUnitsSelected(const QString &newUnits);
};

#endif // STRIPTOOLQUICKCONTROLS_H
