#ifndef BEAMPOSITIONMONITORVIEW_H
#define BEAMPOSITIONMONITORVIEW_H

#include <QWidget>
#include "BeamPositionMonitor.h"

//Adding all the MPlot header files, unsure which I need
#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeriesData.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotSeriesData.h"
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




//View Class for BeamPositionMonitor. Takes a BeamPositionMonitor class object.  Instantizes a MPlotSeriesData, MPlotSeries (for visuals) and is a QWidget



class BeamPositionMonitorView : public QWidget
{
    Q_OBJECT
public:

    //Before this would take two BPM objects one x, y now it'll just take
    // 1 BPM object that contains the two dimensions
    explicit BeamPositionMonitorView(QWidget *parent = 0);


signals:


public slots:


protected slots:
    void updateVectorsFromModel();


protected:
    //Model
    BeamPositionMonitor *bpmXY_;

    //Plot widget that holds the plot used for viewing x,y data
    MPlotWidget *viewWidget_;

    //Plot itself that will be put into the widget
    MPlot *plot_;

    //Scatter point series
    MPlotSeriesBasic *scatter_;

    // 2D (x,y) Data Source containing vectors that get fed from the Model
    MPlotVectorSeriesData *bpmDataSource_;


    //Set up the plot
    void setupPlot();

    //Temporary storage vectors for (x, y) that will come from QList in View. These
    //values will be fed to the MPlotVectorSeriesData
    QVector<qreal> xVector_;
    QVector<qreal> yVector_;
    QList<double> tempX;
    QList<double> tempY;


};

































///Trashing this and re-creating
///
///
///
/*
 *class BeamPositionMonitorView : public QWidget
{
    Q_OBJECT
public:
    explicit BeamPositionMonitorView(BeamPositionMonitor *xBPM,
                                                                          BeamPositionMonitor *yBPM,
                                                                          QWidget *parent = 0);

    //This is elapsed time between (x,y) values.  This may
    //not be needed
    double elapsedTime() const { return elapsedTime_; }
    void setElapsedTime(double newTime) { elapsedTime_ = newTime; }

    //For showing or hiding the widget
    bool getIsActive() const { return isActive_; }
    void  setIsActive(bool value) { isActive_ = value; }

signals:


public slots:
    /*
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

    /*Changed from MPlotRealtimeModel
    MPlotRealtimeModel dataX;
    MPlotRealtimeModel dataY;
    bool isActive_;
    double elapsedTime_;

};
*/



#endif // BEAMPOSITIONMONITORVIEW_H
