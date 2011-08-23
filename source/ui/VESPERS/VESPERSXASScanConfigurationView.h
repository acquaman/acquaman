#ifndef VESPERSXASSCANCONFIGURATIONVIEW_H
#define VESPERSXASSCANCONFIGURATIONVIEW_H

#include "ui/AMScanConfigurationView.h"

#include "ui/AMXASRegionsView.h"
#include "ui/AMRegionsLineView.h"
#include "acquaman/VESPERS/VESPERSXASDacqScanController.h"
#include "acquaman/VESPERS/VESPERSXASScanConfiguration.h"
#include "beamline/VESPERS/VESPERSBeamline.h"

#include <QLineEdit>
#include <QButtonGroup>
#include <QDoubleSpinBox>

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
	/// Handles changing what are acceptable choices for I0 based on It clicks.  Takes in the id of the new It choice.  Passes choice on to the configuration.
	void onItClicked(int id);
	/// Passes on the selection for I0 to the configuration.
	void onI0Clicked(int id) { config_->setIncomingChoice(id); }
	/// Sets the new energy.
	void setEnergy() { VESPERSBeamline::vespers()->mono()->setEo(energy_->value()); }
	/// Handles changes to the energy from outside the program.
	void onEnergyChanged(double energy) { energy_->blockSignals(true); energy_->setValue(energy); energy_->blockSignals(false); }

protected:
	/// Pointer to the specific scan config the view is modifying.
	VESPERSXASScanConfiguration *config_;

	/// This lets you setup regions.
	AMXASRegionsView *regionsView_;
	/// Visual box that shows the current regions.
	AMRegionsLineView *regionsLineView_;

	/// Line edit for changing the name of the scan.
	QLineEdit *scanName_;
	/// Double spin box for changing the energy.
	QDoubleSpinBox *energy_;

	/// Button group for the It ion chamber selection.
	QButtonGroup *ItGroup_;
	/// Button group for the I0 ion chamber selection.
	QButtonGroup *I0Group_;
};

#endif // VESPERSXASSCANCONFIGURATIONVIEW_H
