#ifndef STRIPTOOLVIEW_H
#define STRIPTOOLVIEW_H

#include <QtGui>
#include <QDebug>

#include "StripChart/StripToolPlot.h"
#include "StripChart/StripToolModel.h"
#include "StripChart/StripToolListView.h"
#include "StripChart/StripToolQuickControls.h"
#include "StripChart/ReloadPVDialog.h"

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

protected:
    /// Creates instances of StripToolPlot and StripToolQuickControls, and a checkbox that toggles STQC's visibility.
    void buildUI();

protected slots:
    /// Causes the StripToolsQuickControls to dis/appear as needed.
    void toggleControls(int checkState);

};

#endif // STRIPTOOLVIEW_H
