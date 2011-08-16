#ifndef VESPERSXASSCANCONFIGURATIONVIEW_H
#define VESPERSXASSCANCONFIGURATIONVIEW_H

#include "ui/AMScanConfigurationView.h"

#include "ui/AMXASRegionsView.h"
#include "ui/AMRegionsLineView.h"
#include "acquaman/VESPERS/VESPERSXASDacqScanController.h"
#include "acquaman/VESPERS/VESPERSXASScanConfiguration.h"

#include <QLineEdit>
#include <QButtonGroup>

class VESPERSXASScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.
	/// \param config is the XAS configuration that the view will modify.
	VESPERSXASScanConfigurationView(VESPERSXASScanConfiguration *config, QWidget *parent = 0);

	/// Getter for the configuration.
	const AMScanConfiguration* configuration() const { return config_; }

protected slots:
	/// Handles setting the name of the configuration from the line edit.
	void onScanNameEdited() { config_->setName(scanName_->text()); }
	/// Handles switching which button group is being viewed for Ion chamber selection.
	void onItI0Toggled(int id);

protected:
	/// Pointer to the specific scan config the view is modifying.
	VESPERSXASScanConfiguration *config_;

	/// This lets you setup regions.
	AMXASRegionsView *regionsView_;
	/// Visual box that shows the current regions.
	AMRegionsLineView *regionsLineView_;

	/// Line edit for changing the name of the scan.
	QLineEdit *scanName_;

	/// Button group for the It ion chamber selection.
	QButtonGroup *ItGroup_;
	/// Button group for the I0 ion chamber selection.
	QButtonGroup *I0Group_;
};

#endif // VESPERSXASSCANCONFIGURATIONVIEW_H
