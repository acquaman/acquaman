#ifndef STRIPTOOLVIEW_H
#define STRIPTOOLVIEW_H

#include <QtGui>
#include <QDebug>

#include "StripChart/StripToolPlot.h"
#include "StripChart/StripToolModel.h"
#include "StripChart/StripToolListView.h"
#include "StripChart/StripToolQuickControls.h"

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
    StripToolQuickControls *quickControls_;

    QDir saveDirectory_;
    QString previousPVs_;

    QAction *newPlotAction_;
    QAction *saveDataAction_;
    QAction *quitAction_;

protected:
    /// Used to create the application's actions, but isn't used anymore.
    void createActions();
    /// Creates instances of StripToolPlot and StripToolQuickControls, and a checkbox that toggles STQC's visibility.
    void buildUI();
    /// Searches for "activePVs.txt", which stores basic pv information. Asks the user if they'd like to reload past pvs if the file is found.
    void reloadCheck();

protected slots:
    /// Causes the StripToolsQuickControls to dis/appear as needed.
    void toggleControls(int checkState);
    /// Thought about including an optional legend for the plot, but this isn't implemented.
    void toggleLegend(int checkState);

};

#endif // STRIPTOOLVIEW_H
