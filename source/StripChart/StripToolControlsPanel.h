#ifndef STRIPTOOLCONTROLSPANEL_H
#define STRIPTOOLCONTROLSPANEL_H

#include <QWidget>
#include <QtGui>

#include "StripChart/StripToolModel.h"
#include "StripChart/EntryWidget.h"
#include "StripChart/DoubleButtonWidget.h"
#include "StripChart/TimeEntryWidget.h"

class StripToolControlsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit StripToolControlsPanel(QWidget *parent = 0);
    ~StripToolControlsPanel();

signals:
    void showSidebar();
    void hideSidebar();

public:
    void setModel(StripToolModel *newModel);

protected:
    bool sidebarShown_;
    StripToolModel *model_;
    EntryWidget *nameEntry_;
    DoubleButtonWidget *pauseResume_;
    TimeEntryWidget *timeEntry_;
    QPushButton *sidebarButton_;

protected:

protected slots:
    void onPauseButtonClicked();
    void onResumeButtonClicked();
    void toToggleSidebar();

    void toTestSignal(const QString &signalText);

};

#endif // STRIPTOOLCONTROLSPANEL_H

