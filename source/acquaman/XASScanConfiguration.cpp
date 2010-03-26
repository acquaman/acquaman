#include "XASScanConfiguration.h"

XASScanConfiguration::XASScanConfiguration(QObject *parent) : ScanConfiguration(parent)
{
    qDebug() << "Trying 1.0 " << setExitSlitGap(1.0) << " trying 19.0 " << setExitSlitGap(19.0) << "Trying 500 " << setExitSlitGap(500);
}

double XASScanConfiguration::start(size_t index) const
{
    XASRegion *xasr = region(index);
    if(xasr)
        return xasr->start();
    return -1;
}

double XASScanConfiguration::delta(size_t index) const
{
    XASRegion *xasr = region(index);
    if(xasr)
        return xasr->delta();
    return -1;
}

double XASScanConfiguration::end(size_t index) const
{
    XASRegion *xasr = region(index);
    if(xasr)
        return xasr->end();
    return -1;
}

XASRegion* XASScanConfiguration::region(size_t index) const
{
    if(index < (size_t)regions_.count())
        return regions_.at(index);
    return NULL;
}

bool XASScanConfiguration::setStart(size_t index, double start)
{
    if(index < (size_t)regions_.count()){
        XASRegion* xasr = regions_.at(index);
        xasr->setStart(start);
        return TRUE;
    }
    return FALSE;
}

bool XASScanConfiguration::setDelta(size_t index, double delta)
{
    if(index < (size_t)regions_.count()){
        XASRegion* xasr = regions_.at(index);
        xasr->setDelta(delta);
        return TRUE;
    }
    return FALSE;
}

bool XASScanConfiguration::setEnd(size_t index, double end)
{
    if(index < (size_t)regions_.count()){
        XASRegion* xasr = regions_.at(index);
        xasr->setEnd(end);
        return TRUE;
    }
    return FALSE;
}

bool XASScanConfiguration::setRegion(size_t index, XASRegion *region)
{
    if(index < (size_t)regions_.count()){
        XASRegion *xasr = regions_.at(index);
        xasr = region;
        return TRUE;
    }
    return FALSE;
}

bool XASScanConfiguration::addRegion(size_t index, XASRegion *region)
{
    if(index <= (size_t)regions_.count()){
        regions_.insert(index, region);
        return TRUE;
    }
    return FALSE;
}

bool XASScanConfiguration::addRegion(size_t index, double start, double delta, double end)
{
    XASRegion *xasr = new XASRegion(this);
    xasr->setStart(start);
    xasr->setDelta(delta);
    xasr->setEnd(end);
    return addRegion(index, xasr);
}

bool XASScanConfiguration::deleteRegion(size_t index)
{
    if(index < (size_t)regions_.count()){
        regions_.removeAt(index);
        return TRUE;
    }
    return FALSE;
}

bool XASScanConfiguration::setExitSlitGap(double exitSlitGap){
    if(SGMBeamline::sgm()->exitSlitGap()->valueOutOfRange(exitSlitGap))
        return FALSE;
    exitSlitGap_ = exitSlitGap;
    return TRUE;
}
