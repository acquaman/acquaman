#ifndef STRIPTOOLSELECTOR_H
#define STRIPTOOLSELECTOR_H

#include "MPlot/MPlotTools.h"

class StripToolSelector : public MPlotPlotSelectorTool
{
    Q_OBJECT

public:
    explicit StripToolSelector();
    virtual ~StripToolSelector();

public slots:
    void setSelection(MPlotItem *newSelection);

protected:
    void selectItem(MPlotItem* newSelection);
    void deselectItem();

};

#endif // STRIPTOOLSELECTOR_H
