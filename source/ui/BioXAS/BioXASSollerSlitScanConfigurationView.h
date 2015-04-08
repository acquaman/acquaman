#ifndef BIOXASSOLLERSLITSCANCONFIGURATIONVIEW_H
#define BIOXASSOLLERSLITSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/BioXAS/BioXASSollerSlitScanConfiguration.h"

class BioXASSollerSlitScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASSollerSlitScanConfigurationView(BioXASSollerSlitScanConfiguration *configuration, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSollerSlitScanConfigurationView();

	/// Returns the scan configuration.
	virtual const AMScanConfiguration *configuration() const;

protected:
	/// The soller slit configuration.
	BioXASSollerSlitScanConfiguration *configuration_;
};

#endif // BIOXASSOLLERSLITSCANCONFIGURATIONVIEW_H
