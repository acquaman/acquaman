#ifndef STRIPTOOLVIEW_H
#define STRIPTOOLVIEW_H

#include <QtGui>
#include <QDebug>

#include "StripChart/StripToolPlot.h"
#include "StripChart/StripToolModel.h"
#include "StripChart/StripToolListView.h"
#include "StripChart/StripToolSidePanel.h"
#include "StripChart/ReloadPVDialog.h"
#include "StripChart/StripToolControlsPanel.h"

/// This class is the basic view for the application. The first (read: main) component is the plot, and the second is an instance of "quick controls" that can be shown or hidden using a checkbox. This class also checks whether reloading old pvs is a possibility and confirms with the user whether or not they want to.

class StripToolView : public QWidget
{
    Q_OBJECT
    
public:
    explicit StripToolView(QWidget *parent = 0);
    virtual ~StripToolView();

signals:
    void nameEntered(const QString &newName);
    void plotSelectionChanged(MPlotItem *newSelection);
    void listSelectionChanged(const QModelIndex &newSelection);
    void listItemToEdit(const QModelIndex &toEdit);

public:
    StripToolPlot* plotView() const;
    StripToolListView* listView() const;
    EntryWidget* nameEntry() const;
    WaterfallEntryWidget* waterfallEntry() const;
    TimeEntryWidget* timeEntry() const;

protected:
    StripToolSidePanel* sidePanel() const;
    StripToolControlsPanel* controlPanel() const;

protected slots:
    void toggleSidePanel();

private:
    void buildComponents();
    void makeConnections();
    void defaultSettings();

private:
    bool sidePanelShown_;
    StripToolPlot *plotView_;
    StripToolSidePanel *sidePanel_;
    StripToolControlsPanel *controlPanel_;

};

#endif // STRIPTOOLVIEW_H
