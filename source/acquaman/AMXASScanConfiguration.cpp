#include "AMXASScanConfiguration.h"

AMXASScanConfiguration::AMXASScanConfiguration(QObject *parent) : AMScanConfiguration(parent)
{
    QList<AMControl*> fluxresList;
    fluxresList << SGMBeamline::sgm()->exitSlitGap() << SGMBeamline::sgm()->grating() << SGMBeamline::sgm()->harmonic();
    setExitSlitGap(SGMBeamline::sgm()->exitSlitGap()->value());
    setGrating((SGMBeamline::sgm()->grating()->value() < 1) ? SGMBeamline::lowGrating : ((SGMBeamline::sgm()->grating()->value() < 2) ? SGMBeamline::mediumGrating : SGMBeamline::highGrating));
    setUndulatorTracking(SGMBeamline::sgm()->undulatorTracking()->value());
    setMonoTracking(SGMBeamline::sgm()->monoTracking()->value());
    setExitSlitTracking(SGMBeamline::sgm()->exitSlitTracking()->value());

}

double AMXASScanConfiguration::start(size_t index) const
{
    AMXASRegion *xasr = region(index);
    if(xasr)
        return xasr->start();
    return -1;
}

double AMXASScanConfiguration::delta(size_t index) const
{
    AMXASRegion *xasr = region(index);
    if(xasr)
        return xasr->delta();
    return -1;
}

double AMXASScanConfiguration::end(size_t index) const
{
    AMXASRegion *xasr = region(index);
    if(xasr)
        return xasr->end();
    return -1;
}

AMXASRegion* AMXASScanConfiguration::region(size_t index) const
{
    if(index < (size_t)regions_.count())
        return regions_.at(index);
    return NULL;
}

bool AMXASScanConfiguration::setStart(size_t index, double start)
{
    if(index < (size_t)regions_.count()){
        AMXASRegion* xasr = regions_.at(index);
        xasr->setStart(start);
        return TRUE;
    }
    return FALSE;
}

bool AMXASScanConfiguration::setDelta(size_t index, double delta)
{
    if(index < (size_t)regions_.count()){
        AMXASRegion* xasr = regions_.at(index);
        xasr->setDelta(delta);
        return TRUE;
    }
    return FALSE;
}

bool AMXASScanConfiguration::setEnd(size_t index, double end)
{
    if(index < (size_t)regions_.count()){
        AMXASRegion* xasr = regions_.at(index);
        xasr->setEnd(end);
        return TRUE;
    }
    return FALSE;
}

bool AMXASScanConfiguration::setRegion(size_t index, AMXASRegion *region)
{
    if(index < (size_t)regions_.count()){
        AMXASRegion *xasr = regions_.at(index);
        xasr = region;
        return TRUE;
    }
    return FALSE;
}

bool AMXASScanConfiguration::addRegion(size_t index, AMXASRegion *region)
{
    if(index <= (size_t)regions_.count()){
        regions_.insert(index, region);
        return TRUE;
    }
    return FALSE;
}

bool AMXASScanConfiguration::addRegion(size_t index, double start, double delta, double end)
{
    AMXASRegion *xasr = new AMXASRegion(SGMBeamline::sgm()->energy(), this);
    xasr->setStart(start);
    xasr->setDelta(delta);
    xasr->setEnd(end);
    return addRegion(index, xasr);
}

bool AMXASScanConfiguration::deleteRegion(size_t index)
{
    if(index < (size_t)regions_.count()){
        regions_.removeAt(index);
        return TRUE;
    }
    return FALSE;
}

bool AMXASScanConfiguration::setExitSlitGap(double exitSlitGap){
    if(SGMBeamline::sgm()->exitSlitGap()->valueOutOfRange(exitSlitGap))
        return FALSE;
    exitSlitGap_ = exitSlitGap;
    return TRUE;
}
