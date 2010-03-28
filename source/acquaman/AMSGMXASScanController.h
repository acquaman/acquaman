#ifndef AMSGMXASSCANCONTROLLER_H
#define AMSGMXASSCANCONTROLLER_H

#include "beamline/AMSGMBeamline.h"
#include "AMXASScanConfiguration.h"

class AMSGMXASScanController
{
public:
    AMSGMXASScanController(AMXASScanConfiguration *cfg);

    bool isBeamlineInitialized() { return beamlineInitialized_;}
    virtual bool beamlineInitialize();

protected:
    AMXASScanConfiguration *xasSCfg_;
    bool beamlineInitialized_;
};

#endif // AMSGMXASSCANCONTROLLER_H
