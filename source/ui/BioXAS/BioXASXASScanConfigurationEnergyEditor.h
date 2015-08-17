#ifndef BIOXASXASSCANCONFIGURATIONENERGYEDITOR_H
#define BIOXASXASSCANCONFIGURATIONENERGYEDITOR_H

#include <QLabel>
#include <QDoubleSpinBox>
#include <QLayout>

#include "acquaman/BioXAS/BioXASXASScanConfiguration.h"
#include "ui/BioXAS/BioXASXASScanConfigurationView.h"
#include "ui/BioXAS/BioXASXASScanConfigurationEdgeEditor.h"

class BioXASXASScanConfigurationEnergyEditor : public BioXASXASScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASXASScanConfigurationEnergyEditor(BioXASXASScanConfiguration *configuration, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASXASScanConfigurationEnergyEditor();

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

	/// Updates the scan configuration energy to match the energy spinbox value.
	void updateConfigurationEnergy();

protected:
	/// Double spin box for changing the energy.
	QDoubleSpinBox *energySpinBox_;
	/// Editor for the element and edge selection.
	BioXASXASScanConfigurationEdgeEditor *edgeEditor_;
};

#endif // BIOXASXASSCANCONFIGURATIONENERGYEDITOR_H
