#ifndef STPLOTWIDGET_H
#define STPLOTWIDGET_H

#include <QWidget>

#include "MPlot/MPlot.h"
#include "MPlot/MPlotWidget.h"

class STPlotWidget : public MPlotWidget
{
    Q_OBJECT
public:
    explicit STPlotWidget(QWidget *parent = 0);
    virtual ~STPlotWidget();

signals:

public slots:

protected:

};

#endif // STPLOTWIDGET_H
