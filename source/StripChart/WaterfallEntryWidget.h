#ifndef WATERFALLENTRYWIDGET_H
#define WATERFALLENTRYWIDGET_H

#include <QWidget>
#include <QtGui>

#include "StripChart/StripToolModel.h"

class WaterfallEntryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WaterfallEntryWidget(QWidget *parent = 0);

signals:
    void waterfallChanged(double newWaterfall);

protected:
    QLabel *waterfallLabel_;
    QDoubleSpinBox *waterfallAmount_;

protected:
    void initialize();

protected slots:
    void toSetWaterfallDisplayed(double newDisplay);

};

#endif // WATERFALLENTRYWIDGET_H
