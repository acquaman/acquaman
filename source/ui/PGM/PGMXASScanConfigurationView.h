#ifndef PGMXASSCANCONFIGURATIONVIEW_H
#define PGMXASSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/PGM/PGMXASScanConfiguration.h"

#include "ui/dataman/AMScanAxisView.h"

#include <QLineEdit>

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

protected slots:
	/// Handles setting the name of the configuration from the line edit.
	void onScanNameEdited();

protected:
	/// The configuration.
	PGMXASScanConfiguration *configuration_;

	/// The regions view.
	AMScanAxisView *regionsView_;

	/// The scan name.
	QLineEdit *scanName_;
};

#endif // PGMXASSCANCONFIGURATIONVIEW_H
