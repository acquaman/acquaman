#ifndef BIOXASXASSCANCONFIGURATIONENERGYEDITOR_H
#define BIOXASXASSCANCONFIGURATIONENERGYEDITOR_H

#include <QLabel>
#include <QDoubleSpinBox>
#include <QLayout>

#include "acquaman/BioXAS/BioXASXASScanConfiguration.h"
#include "ui/acquaman/AMScanConfigurationView.h"
#include "ui/BioXAS/BioXASXASScanConfigurationEdgeEditor.h"

class BioXASXASScanConfigurationEnergyEditor : public AMScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASXASScanConfigurationEnergyEditor(BioXASXASScanConfiguration *configuration, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASXASScanConfigurationEnergyEditor();

	/// Returns the scan configuration being viewed.
	const AMScanConfiguration* configuration() const;

signals:
	/// Notifier that the scan configuration being viewed has changed.
	void configurationChanged(BioXASXASScanConfiguration *newConfiguration);

public slots:
	/// Sets the scan configuration being viewed.
	void setConfiguration(BioXASXASScanConfiguration *newConfiguration);

	/// Clears the view.
	void clear();
	/// Updates the view.
	void update();
	/// Refreshes the view.
	void refresh();

protected slots:
	/// Updates the energy spinbox with the configuration's current energy.
	void updateEnergy();

	/// Sets the configuration's energy.
	void setConfigurationEnergy(double newEnergy);
	/// Updates the scan configuration energy to match the energy spinbox value.
	void updateConfigurationEnergy();

protected:
	/// The scan configuration being viewed.
	BioXASXASScanConfiguration *configuration_;

	/// Double spin box for changing the energy.
	QDoubleSpinBox *energySpinBox_;
	/// Editor for the element and line selection.
	BioXASXASScanConfigurationEdgeEditor *edgeEditor_;
};

#endif // BIOXASXASSCANCONFIGURATIONENERGYEDITOR_H
