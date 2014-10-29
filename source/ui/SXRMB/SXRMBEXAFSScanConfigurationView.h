#ifndef SXRMBEXAFSSCANCONFIGURATIONVIEW_H
#define SXRMBEXAFSSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"

class QPushButton;
class QComboBox;
class QCheckBox;
class QDoubleSpinBox;
class QLineEdit;
class QLabel;
class QToolButton;

class AMScanController;
class AMEXAFSScanAxisView;
class AMTopFrame;
class SXRMBEXAFSScanConfiguration;
class AMElement;

class SXRMBEXAFSScanConfigurationView : public AMScanConfigurationView
{
Q_OBJECT
public:
	/// Constructor takes an SXRMB EXAFS scan configuration to view
	SXRMBEXAFSScanConfigurationView(SXRMBEXAFSScanConfiguration *configuration, QWidget *parent = 0);
	/// Destructor
	virtual ~SXRMBEXAFSScanConfigurationView();

	/// Returns the current configuration that we're using
	virtual const AMScanConfiguration* configuration() const;

public slots:
	/// Slot that sets up the regions for standard XANES scans.
	void setupDefaultXANESScanRegions();
	/// Slot that sets up the regions for standard EXAFS scans.
	void setupDefaultEXAFSScanRegions();

protected slots:
	/// Handles setting the name of the configuration from the line edit.
	void onScanNameEdited();
	/// Helper slot that handles the setting the estimated time label.
	void onEstimatedTimeChanged();
	/// Sets the new energy.
	void setEnergy();
	/// Handles choosing a new element when the element button is clicked.
	void onElementChoiceClicked();
	/// Fills in the combo box with lines that can be scanned.
	void fillLinesComboBox(AMElement *el);
	/// Handles changes in the combo box index.
	void onLinesComboBoxIndexChanged(int index);
	/// Handles setting the proper information if the edge is changed.
	void onEdgeChanged();

protected:
	QString convertTimeToString(double time);

protected:
	/// Our scan configuration
	SXRMBEXAFSScanConfiguration *configuration_;

	/// Top frame for consistency
	AMTopFrame *topFrame_;
	/// Regions view for setting/adding/deleting regions
	AMEXAFSScanAxisView *regionsView_;
	/// Automatic button for default XANES configuration on an edge
	QPushButton *autoRegionButton_;
	/// Automatic button for defaults EXAFS configuration on an edge
	QPushButton *pseudoXAFSButton_;

	/// Line edit for the scan name to save in the database
	QLineEdit *scanName_;
	/// Label holding the current estimated time for the scan to complete.  Takes into account extra time per point based on experience on the beamline.
	QLabel *estimatedTime_;

	/// Double spin box for changing the energy.
	QDoubleSpinBox *energy_;
	/// Button used to choose an element to scan over.
	QToolButton *elementChoice_;
	/// The combo box that holds all the lines that can be chosen to scan over.
	QComboBox *lineChoice_;
	/// Label holding the current number of points in a scan.
	QLabel *pointPerScan_;
	/// Label holding the energy space scan range.
	QLabel *scanEnergyRange_;

};

#endif // SXRMBEXAFSSCANCONFIGURATIONVIEW_H
