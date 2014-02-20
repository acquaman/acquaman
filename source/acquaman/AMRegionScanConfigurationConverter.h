#ifndef AMREGIONSCANCONFIGURATIONCONVERTER_H
#define AMREGIONSCANCONFIGURATIONCONVERTER_H

#include "acquaman/AMScanActionControllerScanConfigurationConverter.h"

class AMRegionScanConfiguration;

/// This class extends the functionality of the AMScanActionControllerScanConfigurationConverter to turn any AMRegionScanConfiguration into a viable and runnable scan.
class AMRegionScanConfigurationConverter : public AMScanActionControllerScanConfigurationConverter
{
	Q_OBJECT

public:
	/// Constructor.  Requires a valid scan assembler and an AMRegionScanConfiguration to build from.
	AMRegionScanConfigurationConverter(AMScanActionControllerScanAssembler *scanAssembler = 0, AMRegionScanConfiguration *scanConfiguration = 0, QObject *parent = 0);
	/// Destructor.
	virtual ~AMRegionScanConfigurationConverter();

protected:
	/// Re-implemented to turn an AMRegionScanConfiguration into a runnable scan.
	virtual bool convertImplementation(AMScanActionControllerScanAssembler *scanAssembler, AMScanConfiguration *scanConfiguration);
};

#endif // AMXASSCANCONFIGURATIONCONVERTER_H
