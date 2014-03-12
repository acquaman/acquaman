#ifndef STRIPTOOLSELECTOR_H
#define STRIPTOOLSELECTOR_H

#include "MPlot/MPlotTools.h"

class StripToolSelector : public MPlotPlotSelectorTool
{
    Q_OBJECT

public:
    explicit StripToolSelector();
    virtual ~StripToolSelector();

protected slots:
    void setSelection(MPlotItem* newSelection);

};

#endif // STRIPTOOLSELECTOR_H
