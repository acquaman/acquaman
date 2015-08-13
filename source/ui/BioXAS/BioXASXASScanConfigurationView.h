#ifndef BIOXASXASSCANCONFIGURATIONVIEW_H
#define BIOXASXASSCANCONFIGURATIONVIEW_H

#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QToolButton>
#include <QStringBuilder>
#include <QScrollArea>
#include <QGroupBox>

#include "acquaman/BioXAS/BioXASXASScanConfiguration.h"
#include "ui/acquaman/AMScanConfigurationView.h"
#include "ui/acquaman/AMScanConfigurationViewHolder3.h"
#include "ui/acquaman/AMGenericStepScanConfigurationDetectorsView.h"
#include "util/AMElement.h"

class AMScanController;
class AMEXAFSScanAxisView;
class AMTopFrame;

class BioXASXASScanConfigurationView : public AMScanConfigurationView
{

	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASXASScanConfigurationView(BioXASXASScanConfiguration *configuration, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASXASScanConfigurationView();

	/// Returns the scan configuration being viewed.
    virtual const AMScanConfiguration* configuration() const;

signals:
	/// Notifier that the scan configuration being viewed has changed.
	void scanConfigurationChanged(BioXASXASScanConfiguration *newConfiguration);

public slots:
	/// Sets the XAS scan configuration being viewed.
	void setScanConfiguration(BioXASXASScanConfiguration *newConfiguration);

	/// Clears the view.
	void clear();
	/// Updates the view.
	void update();
	/// Refreshes the view.
	void refresh();

    /// Slot that sets up the regions for standard XANES scans.
    void setupDefaultXANESScanRegions();
    /// Slot that sets up the regions for standard EXAFS scans.
    void setupDefaultEXAFSScanRegions();

protected slots:
    /// Handles setting the name of the configuration from the line edit.
    void onScanNameEdited();
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
    /// Helper slot that handles the setting the estimated time label
    void onEstimatedTimeChanged(double time);

protected:
	/// The configuration being viewed.
	BioXASXASScanConfiguration *configuration_;

	/// Display for the scan name.
    QLineEdit *scanName_;
    /// Double spin box for changing the energy.
    QDoubleSpinBox *energy_;
    /// Button used to choose an element to scan over.
    QToolButton *elementChoice_;
    /// The combo box that holds all the lines that can be chosen to scan over.
    QComboBox *lineChoice_;
	/// The regions view.
	AMEXAFSScanAxisView *regionsView_;
	/// Button that sets the XANES regions.
	QPushButton *autoRegionButton_;
	/// Button that sets the EXAFS regions.
	QPushButton *pseudoXAFSButton_;
    /// Label for time estimation
    QLabel *estimatedTimeLabel_;
	/// The detectors view.
	AMGenericStepScanConfigurationDetectorsView *detectorsView_;
};

#endif // BIOXASXASSCANCONFIGURATIONVIEW_H
