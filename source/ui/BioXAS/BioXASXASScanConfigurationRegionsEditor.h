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
	/// Updates the estimated time label.
	void updateEstimatedTimeLabel();

	/// Sets up the default XANES regions.
	void setupDefaultXANESRegions();
	/// Sets up the default EXAFS regions.
	void setupDefaultEXAFSRegions();

protected:
	/// Creates and returns a default XANES region.
	AMScanAxisEXAFSRegion* createDefaultXANESRegion(double edgeEnergy);
	/// Creates and returns a XANES region.
	AMScanAxisEXAFSRegion* createXANESRegion(double edgeEnergy, double regionStart, double regionStep, double regionEnd, double regionTime) const;
	/// Creates and returns an EXAFS region.
	AMScanAxisEXAFSRegion* createEXAFSRegion(double edgeEnergy, double regionStart, double regionStep, double regionEnd, double regionTime) const;
	/// Creates and returns an EXAFS region with some values set for k space.
	AMScanAxisEXAFSRegion* createEXAFSRegionInKSpace(double edgeEnergy, double regionStart, double regionStep, double regionEnd, double regionTime, double maximumTime) const;

protected:
	/// The regions view.
	AMEXAFSScanAxisView *regionsView_;
	/// Button that sets the XANES regions.
	QPushButton *autoRegionButton_;
	/// Button that sets the EXAFS regions.
	QPushButton *pseudoXAFSButton_;
	/// Label to introduce the time estimation label.
	QLabel *estimatedTimePrompt_;
	/// Label for time estimation.
	QLabel *estimatedTimeLabel_;

	/// The regions view layout.
	QHBoxLayout *regionsViewLayout_;
	/// The buttons layout.
	QHBoxLayout *buttonsLayout_;
	/// The estimated time layout.
	QHBoxLayout *estimatedTimeLayout_;
};

#endif // BIOXASXASSCANCONFIGURATIONREGIONSEDITOR_H
