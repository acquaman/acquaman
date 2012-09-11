#ifndef AMREGIONSCANCONFIGURATIONVIEW_H
#define AMREGIONSCANCONFIGURATIONVIEW_H

#include "acquaman/AMRegionScanConfiguration.h"
#include "ui/acquaman/AMScanConfigurationView.h"

class AMRegionsView;
class AMRegionsLineView;

/// Provides a simple view of an AMRegionScanConfiguration with a table for editing the regions. Can be used within a more detailed scan configuration view, or on its own.
/*! Extend by incorporating inside a more detailed AMScanConfigurationView, or by accessing its layout() and adding widgets within.*/
class AMRegionScanConfigurationView : public AMScanConfigurationView
{
    Q_OBJECT
public:
	/// Constructor. A valid \c configuration must be provided.
	explicit AMRegionScanConfigurationView(AMRegionScanConfiguration* configuration, QWidget *parent = 0);
	virtual ~AMRegionScanConfigurationView();

	/// Returns a pointer to the configuration being edited.
	virtual const AMScanConfiguration* configuration() const { return configuration_; }

signals:

public slots:

protected:
	AMRegionScanConfiguration* configuration_;
	AMRegionsView *regionsView_;
	AMRegionsLineView *regionsLineView_;
};

#endif // AMREGIONSCANCONFIGURATIONVIEW_H
