#ifndef VESPERSEXAFSSCANCONFIGURATIONVIEW_H
#define VESPERSEXAFSSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"

#include "ui/acquaman/AMRegionsView.h"
#include "ui/acquaman/AMRegionsLineView.h"
#include "acquaman/VESPERS/VESPERSEXAFSDacqScanController.h"
#include "acquaman/VESPERS/VESPERSEXAFSScanConfiguration.h"
#include "util/AMElement.h"

#include <QLineEdit>
#include <QTextEdit>
#include <QButtonGroup>
#include <QDoubleSpinBox>
#include <QToolButton>
#include <QComboBox>

class VESPERSEXAFSScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.
	/// \param config is the EXAFS configuration that the view will modify.
	VESPERSEXAFSScanConfigurationView(VESPERSEXAFSScanConfiguration *config, QWidget *parent = 0);

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
	/// Handles changes to the fluorescence detector choice.
	void onFluorescenceChoiceChanged(int id);
	/// Sets the new energy.
	void setEnergy() { config_->setEnergy(energy_->value()); }
	/// Handles choosing a new element when the element button is clicked.
	void onElementChoiceClicked();
	/// Fills in the combo box with lines that can be scanned.
	void fillLinesComboBox(AMElement *el);
	/// Handles changes in the combo box index.
	void onLinesComboBoxIndexChanged(int index);
	/// Sets the current horizontal and vertical positions and saves them in the configuration.
	void setScanPosition() { config_->setPosition(xPosition_->value(), yPosition_->value()); }

protected:
	/// Pointer to the specific scan config the view is modifying.
	VESPERSEXAFSScanConfiguration *config_;

	/// This lets you setup regions.
	AMRegionsView *regionsView_;
	/// Visual box that shows the current regions.
	AMRegionsLineView *regionsLineView_;

	/// Line edit for changing the name of the scan.
	QLineEdit *scanName_;
	/// Double spin box for changing the energy.
	QDoubleSpinBox *energy_;
	/// Button used to choose an element to scan over.
	QToolButton *elementChoice_;
	/// The combo box that holds all the lines that can be chosen to scan over.
	QComboBox *lineChoice_;

	/// The spin box that holds the x coordinate for the scan position.
	QDoubleSpinBox *xPosition_;
	/// The spin box htat holds the y coordinate for the scan position.
	QDoubleSpinBox *yPosition_;

	/// Button group for the It ion chamber selection.
	QButtonGroup *ItGroup_;
	/// Button group for the I0 ion chamber selection.
	QButtonGroup *I0Group_;

	/// The text edit that holds all the names of the regions of interest.
	QTextEdit *roiText_;
};

#endif // VESPERSEXAFSSCANCONFIGURATIONVIEW_H
