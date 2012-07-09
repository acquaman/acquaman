#ifndef VESPERSSPATIALLINESCANCONFIGURATIONVIEW_H
#define VESPERSSPATIALLINESCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/VESPERS/VESPERSSpatialLineScanConfiguration.h"

class VESPERSSpatialLineScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT
public:
	/// Constructor.
	explicit VESPERSSpatialLineScanConfigurationView(VESPERSSpatialLineScanConfiguration *config, QWidget *parent = 0);

	/// Getter for the configuration.
	const AMScanConfiguration* configuration() const { return config_; }

signals:

public slots:

protected:
	/// The scan configuration.
	VESPERSSpatialLineScanConfiguration *config_;
};

#endif // VESPERSSPATIALLINESCANCONFIGURATIONVIEW_H
