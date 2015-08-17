#ifndef IDEAS2DSCANCONFIGURATIONVIEW_H
#define IDEAS2DSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"

#include "acquaman/IDEAS/IDEAS2DScanConfiguration.h"

class IDEAS2DScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.
	IDEAS2DScanConfigurationView(IDEAS2DScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~IDEAS2DScanConfigurationView();

	/// Returns the scan configuration this view represents.
	virtual const AMScanConfiguration *configuration() const;

protected:

	/// Configuration this view represents.
	IDEAS2DScanConfiguration *configuration_;
};

#endif // IDEAS2DSCANCONFIGURATIONVIEW_H
