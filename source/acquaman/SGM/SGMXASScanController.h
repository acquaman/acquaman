#ifndef AMSGMXASSCANCONTROLLER_H
#define AMSGMXASSCANCONTROLLER_H

#include "beamline/SGMBeamline.h"
#include "AMXASScanConfiguration.h"

class SGMXASScanController
{
public:
    SGMXASScanController(AMXASScanConfiguration *cfg);

    bool isBeamlineInitialized() { return beamlineInitialized_;}
    virtual bool beamlineInitialize();

protected:
    AMXASScanConfiguration *xasSCfg_;
    bool beamlineInitialized_;
};

#endif // AMSGMXASSCANCONTROLLER_H
