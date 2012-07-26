#ifndef REIXSXASSCANCONFIGURATIONVIEW_H
#define REIXSXASSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/REIXS/REIXSXASScanConfiguration.h"

class AMSampleSelector;
class AMTopFrame2;

namespace Ui {
class REIXSXASScanConfigurationView;
}

/// Editor widget for a REIXSXASScanConfiguration, used to configure XAS scans to run now or later.
class REIXSXASScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.  If \c config is 0, we create our own (and delete it, when destroyed).  If a valid \c config is provided, we will operate on that one, but not take ownership of it. It must remain valid as long as this widget exists.
	explicit REIXSXASScanConfigurationView(REIXSXASScanConfiguration* config = 0, QWidget *parent = 0);
	virtual ~REIXSXASScanConfigurationView();

	virtual const AMScanConfiguration* configuration() const { return config_; }

protected:
	REIXSXASScanConfiguration* config_;

protected slots:
	/// Determines whether to enable the polarization angle (only if the polarization is enabled, and set to Linear Inclined).
	void reviewPolarizationAngleBoxEnabled();
	/// Computes the expected acquisition time when the regions change.
	void onRegionsChanged();

private:
	Ui::REIXSXASScanConfigurationView *ui;
	AMSampleSelector* sampleSelector_;
	AMTopFrame2* topFrame_;
};

#endif // REIXSXASSCANCONFIGURATIONVIEW_H
