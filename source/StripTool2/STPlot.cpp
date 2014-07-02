#include "STPlot.h"

STPlot::STPlot(const QRectF &rect, QGraphicsItem *parent) : MPlot(rect, parent)
{
    timeValue_ = 10;
    timeUnits_ = Seconds;
}

STPlot::~STPlot()
{

}
