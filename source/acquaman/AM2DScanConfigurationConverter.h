#ifndef AM2DSCANCONFIGURATIONCONVERTER_H
#define AM2DSCANCONFIGURATIONCONVERTER_H

#include "acquaman/AMScanActionControllerScanConfigurationConverter.h"

class AM2DScanConfiguration;

#define AM2DSCANCONFIGURATIONCONVERTER_INVALID_CONFIGURATION 689000
#define AM2DSCANCONFIGURATIONCONVERTER_INVALID_XAXIS 689001
#define AM2DSCANCONFIGURATIONCONVERTER_INVALID_YAXIS 689002
#define AM2DSCANCONFIGURATIONCONVERTER_COULD_NOT_ADD_X_AXIS 689003
#define AM2DSCANCONFIGURATIONCONVERTER_COULD_NOT_ADD_Y_AXIS 689004
#define AM2DSCANCONFIGURATIONCONVERTER_COULD_NOT_ADD_DETECTOR 689005

/// This class extends the functionality of the AMScanActionControllerScanConfigurationConverter to build the appropriate axes for a 2D scan.
class AM2DScanConfigurationConverter : public AMScanActionControllerScanConfigurationConverter
{
	Q_OBJECT

public:
	/// Constructor.  Requires a valid scan assembler and an AM2DScanConfiguration.
	AM2DScanConfigurationConverter(AMScanActionControllerScanAssembler *scanAssembler = 0, AM2DScanConfiguration *scanConfiguration = 0, QObject *parent = 0);
	/// Destructor.
	virtual ~AM2DScanConfigurationConverter(){}

protected:
	/// Re-implemented to turn an AM2DScanConfiguration into a runnable scan.
	virtual bool convertImplementation(AMScanActionControllerScanAssembler *scanAssembler, AMScanConfiguration *scanConfiguration);
};

#endif // AM2DSCANCONFIGURATIONCONVERTER_H
