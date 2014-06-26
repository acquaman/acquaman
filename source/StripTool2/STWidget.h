#ifndef STWIDGET_H
#define STWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QDialog>

#include "beamline/AMPVControl.h"

#include "MPlot/MPlotSeriesData.h"
#include "MPlot/MPlotSeries.h"

#include "STVariable.h"
#include "STPlotWidget.h"
#include "STPlotEditor.h"

class STWidget : public QWidget
{
    Q_OBJECT

public:
    STWidget(QWidget *parent = 0);
    ~STWidget();

public slots:
    void showPlotEditor(bool show);

protected slots:
    void onRingCurrentConnected(bool isConnected);
    void onRingCurrentValueChanged(double newValue);

protected:
    QLabel *ringCurrentLabel_;
    AMProcessVariable *ringCurrentControl_;
    STVariable *ringCurrent_;

    STPlotWidget *plotWidget_;

    STPlotEditor *plotEditor_;
};

#endif // STWIDGET_H
