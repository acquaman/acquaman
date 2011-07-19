#ifndef VESPERSXASSCANCONFIGURATIONVIEW_H
#define VESPERSXASSCANCONFIGURATIONVIEW_H

#include "ui/AMScanConfigurationView.h"

#include "ui/AMXASRegionsView.h"
#include "ui/AMRegionsLineView.h"
#include "acquaman/VESPERS/VESPERSXASDacqScanController.h"
#include "acquaman/VESPERS/VESPERSXASScanConfiguration.h"

class VESPERSXASScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.
	/// \param config is the XAS configuration that the view will modify.
	VESPERSXASScanConfigurationView(VESPERSXASScanConfiguration *config, QWidget *parent = 0);

	/// Getter for the configuration.
	const AMScanConfiguration* configuration() const;

protected:
	/// Pointer to the specific scan config the view is modifying.
	VESPERSXASScanConfiguration *config_;

	/// This lets you setup regions.
	AMXASRegionsView *regionsView_;
	/// Visual box that shows the current regions.
	AMRegionsLineView *regionsLineView_;
};

#endif // VESPERSXASSCANCONFIGURATIONVIEW_H
