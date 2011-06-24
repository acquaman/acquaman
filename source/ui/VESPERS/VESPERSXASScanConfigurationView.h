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
	VESPERSXASScanConfigurationView(VESPERSXASScanConfiguration *config, QWidget *parent = 0);

	const AMScanConfiguration* configuration() const;

protected:
	VESPERSXASScanConfiguration *config_;

	AMXASRegionsView *regionsView_;
	AMRegionsLineView *regionsLineView_;
};

#endif // VESPERSXASSCANCONFIGURATIONVIEW_H
