#ifndef STRIPTOOLSELECTOR_H
#define STRIPTOOLSELECTOR_H

#include "MPlot/MPlotTools.h"

class StripToolSelector : public MPlotPlotSelectorTool
{
    Q_OBJECT

public:
    explicit StripToolSelector();
    ~StripToolSelector();

public slots:
    void setSelection(MPlotItem *newSelection);

protected slots:
    void onItemDeselected();

protected:
    void selectItem(MPlotItem* newSelection);
    void deselectItem();

private:
    void makeConnections();

};

#endif // STRIPTOOLSELECTOR_H
