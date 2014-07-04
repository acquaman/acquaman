#ifndef STPLOTWIDGET_H
#define STPLOTWIDGET_H

#include <QWidget>
#include <QLayout>
#include <QDebug>

#include "MPlot/MPlot.h"
#include "MPlot/MPlotWidget.h"

class STPlotWidget : public MPlotWidget
{
    Q_OBJECT
public:
    explicit STPlotWidget(QWidget *parent = 0);
    virtual ~STPlotWidget();

    QString plotName() const;
    bool plotNameVisible() const;

signals:
    void plotNameChanged(const QString &name);

public slots:
    void setPlotName(const QString &newName);
    void showPlotName(bool show);

protected:
    QString plotName_;
    bool plotNameVisible_;

};

#endif // STPLOTWIDGET_H
