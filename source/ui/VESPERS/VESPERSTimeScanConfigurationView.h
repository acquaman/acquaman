#ifndef VESPERSTIMESCANCONFIGURATIONVIEW_H
#define VESPERSTIMESCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/VESPERS/VESPERSTimeScanConfiguration.h"

class VESPERSTimeScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT

public:
	VESPERSTimeScanConfigurationView(VESPERSTimeScanConfiguration *config, QWidget *parent = 0);

	/// Getter for the configuration.
	const AMScanConfiguration* configuration() const { return config_; }

protected:
	/// Pointer to the specific scan config the view is modifying.
	VESPERSTimeScanConfiguration *config_;
};

#endif // VESPERSTIMESCANCONFIGURATIONVIEW_H
