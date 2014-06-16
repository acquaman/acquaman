#ifndef STWIDGET_H
#define STWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLayout>

#include "beamline/AMPVControl.h"
#include "source/analysis/AM1DSummingAB.h"
#include "dataman/datasource/AMDataSourceSeriesData.h"
#include "dataman/datasource/AMProcessVariableDataSource.h"

#include "MPlot/MPlotSeriesData.h"
#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlot.h"

#include "STRealtimeModelAB.h"

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

    AMProcessVariable *ringCurrentControl_;
    QLabel *ringCurrentLabel_;

    MPlotRealtimeModel *ringCurrentModel1_;
    STRealtimeModelAB *ringCurrentModel2_;

    MPlotSeriesBasic *ringCurrentSeries1_;
    MPlotSeriesBasic *ringCurrentSeries2_;

    MPlotWidget *plotWidget_;
    MPlot *plot_;
};

#endif // STWIDGET_H
