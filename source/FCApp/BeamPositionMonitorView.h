#ifndef BEAMPOSITIONMONITORVIEW_H
#define BEAMPOSITIONMONITORVIEW_H

#include <QWidget>
#include "BeamPositionMonitor.h"

//Adding all the MPlot header files, unsure which I need
#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeriesData.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotImageData.h"
#include "MPlot/MPlotImage.h"
#include "MPlot/MPlotPoint.h"
#include <QTableView>
#include <QPen>
#include <QBrush>
#include <QGradientStops>
#include <QGradientStop>
#include <QPrinter>
#include <QPrintDialog>
#include <cmath>
#include "MPlot/MPlotTools.h"
#include "MPlot/MPlotRectangle.h"

#include <QTimer>
#include <QTime>
//View Class for BeamPositionMonitor. Takes in two 1-dim BPM variables, representing (x, y) coordinates and displays using MPlot



class BeamPositionMonitorView : public QWidget
{
    Q_OBJECT
public:
    explicit BeamPositionMonitorView(BeamPositionMonitor *xBPM,
                                                                          BeamPositionMonitor *yBPM,
                                                                          QWidget *parent = 0);

    double elapsedTime_;

    bool getIsActive() const { return isActive_; }
    void  setIsActive(bool value) { isActive_ = value; }

signals:


public slots:
    void setXValue(double value);
    void setYValue(double value);
    void updatePlot();
    void removePlotPoint();
    void showBPM();


protected:
    BeamPositionMonitor *xBPM_;
    BeamPositionMonitor *yBPM_;

    double xValue_;
    double yValue_;

    MPlotWidget plotWindowX_;
    MPlotWidget plotWindowY_;
    MPlot plotX_;
    MPlot plotY_;
    
    MPlotRealtimeModel dataX;
    MPlotRealtimeModel dataY;

    bool isActive_;

};

#endif // BEAMPOSITIONMONITORVIEW_H
