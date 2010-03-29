#ifndef ACQMAN_SGMXASSCANCONFIGURATION_H
#define ACQMAN_SGMXASSCANCONFIGURATION_H

#include "acquaman/AMXASScanConfiguration.h"
#include "SGMScanConfiguration.h"

class SGMXASScanConfiguration : public AMXASScanConfiguration, public SGMScanConfiguration
{
    Q_OBJECT
public:
    SGMXASScanConfiguration(QObject *parent=0);

public slots:
    virtual bool addRegion(size_t index, AMXASRegion *region) {return AMXASScanConfiguration::addRegion(index, region);}
    virtual bool addRegion(size_t index, double start, double delta, double end);
};

#endif // ACQMAN_SGMXASSCANCONFIGURATION_H
