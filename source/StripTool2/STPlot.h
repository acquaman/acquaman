#ifndef STPLOT_H
#define STPLOT_H

#include <QObject>

#include "MPlot/MPlot.h"

class STPlot : public MPlot
{

public:
    enum TimeUnits {mSeconds = 0, Seconds = 1, Minutes = 2, Hours = 3};

    explicit STPlot(const QRectF& rect = QRectF(0, 0, 100, 100), QGraphicsItem *parent = 0);
    virtual ~STPlot();

signals:

public slots:

protected:
    int timeValue_;
    TimeUnits timeUnits_;

};

#endif // STPLOT_H
