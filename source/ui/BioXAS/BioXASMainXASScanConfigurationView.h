#ifndef BIOXASMAINXASSCANCONFIGURATIONVIEW_H
#define BIOXASMAINXASSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/BioXAS/BioXASMainXASScanConfiguration.h"
#include "util/AMElement.h"

#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QToolButton>

class AMScanController;
class AMEXAFSScanAxisView;
class AMTopFrame;

class BioXASMainXASScanConfigurationView : public AMScanConfigurationView
{

Q_OBJECT

public:
    BioXASMainXASScanConfigurationView(BioXASMainXASScanConfiguration *configuration, QWidget *parent = 0);
    virtual ~BioXASMainXASScanConfigurationView();

    virtual const AMScanConfiguration* configuration() const;

public slots:
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
    void onEstimatedTimeChanged();

protected:
    BioXASMainXASScanConfiguration *configuration_;

    AMTopFrame *topFrame_;
    AMEXAFSScanAxisView *regionsView_;
    QPushButton *autoRegionButton_;
    QPushButton *pseudoXAFSButton_;

    QLineEdit *scanName_;

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
    /// Label for time estimation
    QLabel *estimatedTimeLabel_;
};

#endif // BIOXASMAINXASSCANCONFIGURATIONVIEW_H
