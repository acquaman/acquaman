#ifndef STRIPTOOLCONTROLSPANEL_H
#define STRIPTOOLCONTROLSPANEL_H

#include <QWidget>
#include <QtGui>

#include "StripChart/StripToolModel.h"
#include "StripChart/EntryWidget.h"
#include "StripChart/DoubleButtonWidget.h"
#include "StripChart/TimeEntryWidget.h"
#include "StripChart/WaterfallEntryWidget.h"

class StripToolControlsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit StripToolControlsPanel(QWidget *parent = 0);
    ~StripToolControlsPanel();

public:
    EntryWidget* nameEntry() const;
    WaterfallEntryWidget* waterfallEntry() const;
    TimeEntryWidget* timeEntry() const;
    QPushButton* sidebarButton() const;

public slots:

private:
    void buildComponents();
    void makeConnections();
    void defaultSettings();

private:
    EntryWidget *nameEntry_;
//    DoubleButtonWidget *pauseResume_;
    TimeEntryWidget *timeEntry_;
    QPushButton *sidebarButton_;
    WaterfallEntryWidget *waterfallEntry_;

};

#endif // STRIPTOOLCONTROLSPANEL_H

