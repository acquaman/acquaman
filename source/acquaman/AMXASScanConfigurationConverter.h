#ifndef AMXASSCANCONFIGURATIONCONVERTER_H
#define AMXASSCANCONFIGURATIONCONVERTER_H

#include "acquaman/AMScanActionControllerScanConfigurationConverter.h"

class AMXASScanConfiguration;

class AMXASScanConfigurationConverter : public AMScanActionControllerScanConfigurationConverter
{
Q_OBJECT
public:
	AMXASScanConfigurationConverter(AMScanActionControllerScanAssembler *scanAssembler = 0, AMXASScanConfiguration *scanConfiguration = 0, QObject *parent = 0);

protected:
	virtual bool convertImplementation(AMScanActionControllerScanAssembler *scanAssembler, AMScanConfiguration *scanConfiguration);
};

#endif // AMXASSCANCONFIGURATIONCONVERTER_H
