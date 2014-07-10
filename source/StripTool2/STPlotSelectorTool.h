#ifndef STPLOTSELECTORTOOL_H
#define STPLOTSELECTORTOOL_H

#include "MPlot/MPlotTools.h"

class STPlotSelectorTool : public MPlotPlotSelectorTool
{
    Q_OBJECT

public:
    explicit STPlotSelectorTool();
    virtual ~STPlotSelectorTool();

public slots:
    void setSelection(MPlotItem *newSelection);

protected:

};

#endif // STPLOTSELECTORTOOL_H
