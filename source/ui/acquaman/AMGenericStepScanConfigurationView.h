#ifndef AMGENERICSTEPSCANCONFIGURATIONVIEW_H
#define AMGENERICSTEPSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/AMGenericStepScanConfiguration.h"

#include <QLabel>

class AMGenericStepScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.
	AMGenericStepScanConfigurationView(AMGenericStepScanConfiguration *configuration, QWidget *parent = 0);
	/// Destructor.
	~AMGenericStepScanConfigurationView(){}

	/// Getter for the configuration.
	const AMScanConfiguration *configuration() const { return configuration_; }

protected:
	/// Pointer to the generic scan configuration.
	AMGenericStepScanConfiguration *configuration_;

	/// Pointer to the label that holds the current map settings.
	QLabel *scanInformation_;
	/// Label holding the current estimated time for the scan to complete.  Takes into account extra time per point based on experience on the beamline.
	QLabel *estimatedTime_;
};

#endif // AMGENERICSTEPSCANCONFIGURATIONVIEW_H
