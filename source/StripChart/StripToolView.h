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
    explicit StripToolView(QWidget *parent = 0, StripToolModel *model = 0);
    ~StripToolView();

signals:
    void reloadPVs(bool reload);

protected:
    StripToolModel *model_;
    StripToolPlot *plotView_;
    StripToolSidePanel *sidePanel_;
    StripToolControlsPanel *controlPanel_;

protected:
    /// Creates instances of StripToolPlot, StripToolSidePanel, and StripToolControlsPanel.
    void buildUI();

};

#endif // STRIPTOOLVIEW_H
