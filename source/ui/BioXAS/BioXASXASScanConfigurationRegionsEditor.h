#ifndef BIOXASXASSCANCONFIGURATIONREGIONSEDITOR_H
#define BIOXASXASSCANCONFIGURATIONREGIONSEDITOR_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLayout>

#include "acquaman/BioXAS/BioXASXASScanConfiguration.h"
#include "ui/BioXAS/BioXASXASScanConfigurationView.h"
#include "ui/dataman/AMEXAFSScanAxisView.h"

class BioXASXASScanConfigurationRegionsEditor : public BioXASXASScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASXASScanConfigurationRegionsEditor(BioXASXASScanConfiguration *configuration, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASXASScanConfigurationRegionsEditor();

public slots:
	/// Sets the scan configuration being viewed.
	virtual void setConfiguration(BioXASXASScanConfiguration *newConfiguration);

	/// Clears the view.
	virtual void clear();
	/// Updates the view with the current configuration information.
	virtual void update();
	/// Refreshes the view.
	virtual void refresh();

protected slots:
	/// Updates the regions view.
	void updateRegionsView();
	/// Updates the estimated time label.
	void updateEstimatedTimeLabel();
	/// Updates the XANES button.
	void updateXANESButton();
	/// Updates the EXAFS button.
	void updateEXAFSButton();

	/// Handles setting up a default XANES region, if the current configuration is valid.
	void onXANESButtonClicked();
	/// Handles setting up default EXAFS regions, if the current configuration is valid.
	void onEXAFSButtonClicked();

	/// Handles making the appropriate connections when the configuration has a new scan axis.
	void connectScanAxis(AMScanAxis *newAxis);
	/// Handles disconnecting the appropriate connections when the configuration has removed a scan axis.
	void disconnectScanAxis(AMScanAxis *axis);

protected:
	/// Creates and returns a regions view for the given configuration.
	static AMEXAFSScanAxisView* createRegionsView(BioXASXASScanConfiguration *configuration);

protected:
	/// The regions view.
	AMEXAFSScanAxisView *regionsView_;
	/// Button that sets the XANES regions.
	QPushButton *xanesButton_;
	/// Button that sets the EXAFS regions.
	QPushButton *exafsButton_;
	/// Label for time estimation.
	QLabel *estimatedTimeLabel_;

	/// The regions view layout.
	QHBoxLayout *regionsViewLayout_;
};

#endif // BIOXASXASSCANCONFIGURATIONREGIONSEDITOR_H
