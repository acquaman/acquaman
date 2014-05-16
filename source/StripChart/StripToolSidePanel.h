#ifndef STRIPTOOLSIDEPANEL_H
#define STRIPTOOLSIDEPANEL_H

#include <QWidget>
#include <QtGui>

#include "StripChart/StripToolModel.h"
#include "StripChart/StripToolListView.h"
#include "StripChart/EntryWidget.h"
#include "StripChart/TimeEntryWidget.h"
#include "StripChart/WaterfallEntryWidget.h"

/// This class is a sort of optional sidebar that provides a number of ways for the user to interact with an added pv, as well as a quick way to add a new one.
class StripToolSidePanel : public QWidget
{
    Q_OBJECT

public:
    explicit StripToolSidePanel(QWidget *parent = 0);
    ~StripToolSidePanel();
    friend class StripToolView;
    
signals:

public:
    StripToolListView* listView() const;
    EntryWidget *nameEntry() const;
    TimeEntryWidget *timeEntry() const;
    WaterfallEntryWidget *waterfallEntry() const;

private:
    void buildComponents();
    void makeConnections();
    void defaultSettings();

private:
    StripToolListView *listView_;
    EntryWidget *nameEntry_;
    TimeEntryWidget *timeEntry_;
    WaterfallEntryWidget *waterfallEntry_;

};

#endif // STRIPTOOLSIDEPANEL_H
