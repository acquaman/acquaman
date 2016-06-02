#ifndef PGMXASSCANCONFIGURATIONVIEW_H
#define PGMXASSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/PGM/PGMXASScanConfiguration.h"

/// The scan configuration view for XAS on PGM.
class PGMXASScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.
	PGMXASScanConfigurationView(PGMXASScanConfiguration *configuration, QWidget *parent = 0);
	/// Destructor.
	virtual ~PGMXASScanConfigurationView(){}

	/// Returns the configuration.
	virtual AMScanConfiguration *configuration() const { return configuration_; }

signals:

public slots:

protected:
	/// The configuration.
	PGMXASScanConfiguration *configuration_;
};

#endif // PGMXASSCANCONFIGURATIONVIEW_H
