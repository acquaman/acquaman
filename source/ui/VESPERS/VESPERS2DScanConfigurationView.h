#ifndef VESPERS2DSCANCONFIGURATIONVIEW_H
#define VESPERS2DSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/VESPERS/VESPERS2DDacqScanController.h"
#include "acquaman/VESPERS/VESPERS2DScanConfiguration.h"

#include <QLineEdit>
#include <QTextEdit>
#include <QButtonGroup>
#include <QDoubleSpinBox>
#include <QToolButton>
#include <QComboBox>
#include <QLabel>

/// This class builds the view for configuring a 2D map scan for the VESPERS beamline.
class VESPERS2DScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.  \param config is the 2D configuration that the view will modify.
	VESPERS2DScanConfigurationView(VESPERS2DScanConfiguration *config, QWidget *parent = 0);

	/// Getter for the configuration.
	const AMScanConfiguration *configuration() const { return config_; }

protected slots:
	/// Handles setting the name of the configuration from the line edit.
	void onScanNameEdited() { config_->setName(scanName_->text()); }
	/// Passes on the selection for I0 to the configuration.
	void onI0Clicked(int id) { config_->setIncomingChoice(id); }
	/// Handles changes to the fluorescence detector choice.
	void onFluorescenceChoiceChanged(int id);
	/// Helper slot that handles the setting the estimated time label.
	void onEstimatedTimeChanged();

	/// Handles the context menu.
	void onCustomContextMenuRequested(QPoint pos);

protected:
	/// Helper method that takes a time in seconds and returns a string of d:h:m:s.
	QString convertTimeToString(double time);

	/// Pointer to the specific scan config the view is modifying.
	VESPERS2DScanConfiguration *config_;

	/// Line edit for changing the name of the scan.
	QLineEdit *scanName_;
	/// Label holding the current estimated time for the scan to complete.  Takes into account extra time per point based on experience on the beamline.
	QLabel *estimatedTime_;
	/// Button group for the I0 ion chamber selection.
	QButtonGroup *I0Group_;
	/// The text edit that holds all the names of the regions of interest.
	QTextEdit *roiText_;

	/// A label holding text for the the time offset spin box.
	QLabel *timeOffsetLabel_;
	/// A spin box holding the time offset.
	QDoubleSpinBox *timeOffset_;
};

#endif // VESPERS2DSCANCONFIGURATIONVIEW_H
