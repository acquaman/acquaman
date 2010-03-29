#include "SGMXASScanConfiguration.h"

SGMXASScanConfiguration::SGMXASScanConfiguration(QObject *parent) : AMXASScanConfiguration(parent) , SGMScanConfiguration()
{
}

bool SGMXASScanConfiguration::addRegion(size_t index, double start, double delta, double end)
{
    AMXASRegion *xasr = new AMXASRegion(SGMBeamline::sgm()->energy(), this);
    xasr->setStart(start);
    xasr->setDelta(delta);
    xasr->setEnd(end);
    return addRegion(index, xasr);
}
