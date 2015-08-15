#ifndef BIOXASXASSCANCONFIGURATIONREGIONSEDITOR_H
#define BIOXASXASSCANCONFIGURATIONREGIONSEDITOR_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLayout>

#include "acquaman/BioXAS/BioXASXASScanConfiguration.h"
#include "ui/dataman/AMEXAFSScanAxisView.h"

class BioXASXASScanConfigurationRegionsEditor : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASXASScanConfigurationRegionsEditor(BioXASXASScanConfiguration *configuration, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASXASScanConfigurationRegionsEditor();

	/// Returns the scan configuration being viewed.
	BioXASXASScanConfiguration* configuration() const { return configuration_; }

signals:
	/// Notifier that the scan configuration being viewed has changed.
	void configurationChanged(BioXASXASScanConfiguration *newConfiguration);

public slots:
	/// Sets the scan configuration being viewed.
	void setConfiguration(BioXASXASScanConfiguration *newConfiguration);

	/// Clears the view.
	void clear();
	/// Updates the view with the current configuration information.
	void update();
	/// Refreshes the view.
	void refresh();

protected slots:
	/// Clears the regions view.
	void clearRegionsView();

	/// Updates the regions view.
	void updateRegionsView();
	/// Updates the regions buttons.
	void updateRegionsButtons();
	/// Updates the estimated time label.
	void updateEstimatedTimeLabel();

protected:
	/// The scan configuration being viewed.
	BioXASXASScanConfiguration* configuration_;

	/// The regions view.
	AMEXAFSScanAxisView *regionsView_;
	/// Button that sets the XANES regions.
	QPushButton *autoRegionButton_;
	/// Button that sets the EXAFS regions.
	QPushButton *pseudoXAFSButton_;
	/// Label for time estimation.
	QLabel *estimatedTimeLabel_;
	/// The main layout.
	QVBoxLayout *layout_;
};

#endif // BIOXASXASSCANCONFIGURATIONREGIONSEDITOR_H
