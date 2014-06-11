#ifndef STWIDGET_H
#define STWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLayout>

#include "beamline/AMPVControl.h"
#include "source/analysis/AM1DSummingAB.h"
#include "dataman/datasource/AMDataSourceSeriesData.h"

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

    MPlotRealtimeModel *ringCurrentModel1_;
    MPlotSeriesBasic *ringCurrentSeries1_;

    MPlotRealtimeModel *ringCurrentModel2_;
    MPlotSeriesBasic *ringCurrentSeries2_;

    AM1DSummingAB *summedData_;
    MPlotSeriesBasic *ringCurrentSeries3_;

    MPlotWidget *plotWidget_;
    MPlot *plot_;
};

#endif // STWIDGET_H
