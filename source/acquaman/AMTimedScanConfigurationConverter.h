#ifndef AMTIMEDSCANCONFIGURATIONCONVERTER_H
#define AMTIMEDSCANCONFIGURATIONCONVERTER_H

#include "acquaman/AMScanActionControllerScanConfigurationConverter.h"

class AMTimedRegionScanConfiguration;

#define AMTIMEDSCANCONFIGURATIONCONVERTER_INVALID_CONFIGURATION 683000
#define AMTIMEDSCANCONFIGURATIONCONVERTER_INVALID_SCAN_AXIS 683001
#define AMTIMEDSCANCONFIGURATIONCONVERTER_COULD_NOT_ADD_DETECTOR 683002

/// This class extends the functionality of the AMScanActionControllerScanConfigurationConverter to turn the scan configuration into a proper action tree.
class AMTimedScanConfigurationConverter : public AMScanActionControllerScanConfigurationConverter
{
	Q_OBJECT

public:
	/// Constructor.  Requires a valid scan assember and an AMTimedRegionScanConfiguration to build from.
	AMTimedScanConfigurationConverter(AMScanActionControllerScanAssembler *scanAssembler = 0, AMTimedRegionScanConfiguration *scanConfiguration = 0, QObject *parent = 0);
	/// Destructor.
	virtual ~AMTimedScanConfigurationConverter(){}

protected:
	/// Re-implemented to turn an AMRegionScanConfiguration into a runnable scan.
	virtual bool convertImplementation(AMScanActionControllerScanAssembler *scanAssembler, AMScanConfiguration *scanConfiguration);
};

#endif // AMTIMEDSCANCONFIGURATIONCONVERTER_H
