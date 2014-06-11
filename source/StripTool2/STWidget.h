#ifndef STWIDGET_H
#define STWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLayout>

#include "beamline/AMPVControl.h"

#include "MPlot/MPlotSeriesData.h"
#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlot.h"

class STWidget : public QWidget
{
    Q_OBJECT

public:
    STWidget(QWidget *parent = 0);
    ~STWidget();

protected slots:
    void onRingCurrentValueChanged(double newValue);
    void onRingCurrentConnected(bool isConnected);

protected:
    int dataCount_;
    int displayCount_;

    AMReadOnlyPVControl *ringCurrentControl_;
    QLabel *ringCurrentLabel_;

    MPlotRealtimeModel *ringCurrentModel_;
    MPlotSeriesBasic *ringCurrentSeries_;

    MPlotWidget *plotWidget_;
    MPlot *plot_;
};

#endif // STWIDGET_H
