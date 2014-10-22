#ifndef BEAMPOSITIONMONITORVIEW_H
#define BEAMPOSITIONMONITORVIEW_H

#include <QWidget>
#include "BeamPositionMonitor.h"
#include "BeamlineShutters.h"
#include "BeamlineShutterView.h"
//#include "sidebar.h"
#include <QWidget>
#include <QLCDNumber>
#include <QLabel>
#include <QPushButton>


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

//
#include <QtGui/QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QIcon>
#include <QAction>
//




//View Class for BeamPositionMonitor. Takes a BeamPositionMonitor class object.  Instantizes a MPlotSeriesData, MPlotSeries (for visuals) and is a QWidget



class BeamPositionMonitorView : public QWidget
{
    Q_OBJECT
public:

    //Before this would take two BPM objects one x, y now it'll just take
    // 1 BPM object that contains the two dimensions
    explicit BeamPositionMonitorView(QWidget *parent = 0);

   //Side bar
    void addAction(QAction *action);
    QAction *addAction(const QString &text, const QIcon &icon = QIcon());
  /////////////////

 bool displayBPM;
 bool displayShutters;

signals:


public slots:


protected slots:
    void updateVectorsFromModel();



protected:

    BeamlineShutterView *bmitView_ ;

    BeamlineShutterView *sylmandView_;
    BeamlineShutterView *hxmaView_;
    BeamlineShutterView *sxrmbView_;
    BeamlineShutterView *vespersView_;
    BeamlineShutterView *cmcfidView_;
    BeamlineShutterView *cmcfbmView_;
    BeamlineShutterView *ideasView_;
    BeamlineShutterView *smView_;
    BeamlineShutterView *sgmpgmView_;

    QWidget *wid;

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


    // Side bar
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    QSize minimumSizeHint() const;
    //Determine which action the mouse is selecting

    QAction *actionAt(const QPoint &at);

private:
    QList<QAction*> _actions;
    QAction *_pressedAction;
    QAction *_checkedAction;

};

#endif
