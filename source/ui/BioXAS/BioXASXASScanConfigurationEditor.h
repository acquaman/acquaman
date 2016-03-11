#ifndef BIOXASXASSCANCONFIGURATIONEDITOR_H
#define BIOXASXASSCANCONFIGURATIONEDITOR_H

#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include <QTabWidget>

#include "acquaman/BioXAS/BioXASXASScanConfiguration.h"
#include "ui/acquaman/AMScanConfigurationView.h"
#include "ui/acquaman/AMGenericStepScanConfigurationDetectorsView.h"
#include "ui/BioXAS/BioXASXASScanConfigurationEdgeEditor.h"
#include "ui/BioXAS/BioXASXASScanConfigurationRegionsEditor.h"

class BioXASXASScanConfigurationEditor : public BioXASXASScanConfigurationView
{

	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASXASScanConfigurationEditor(BioXASXASScanConfiguration *configuration, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASXASScanConfigurationEditor();

public slots:
	/// Sets the XAS scan configuration being viewed.
	virtual void setConfiguration(BioXASXASScanConfiguration *newConfiguration);

	/// Clears the view.
	virtual void clear();
	/// Updates the view.
	virtual void update();
	/// Refreshes the view.
	virtual void refresh();

protected slots:
	/// Updates the displayed scan name to correspond to the configuration's scan name.
	void updateNameLineEdit();
	/// Updates the displayed energy to correspond to the configuration's energy.
	void updateEnergySpinBox();

	/// Updates the scan configuration name.
	void updateConfigurationName();
	/// Updates the configuration's energy.
	void updateConfigurationEnergy();

protected:
	/// Scan name editor.
	QLineEdit *nameLineEdit_;
	/// Energy spinbox.
	QDoubleSpinBox *energySpinBox_;
	/// Edge editor.
	BioXASXASScanConfigurationEdgeEditor *edgeEditor_;
	/// Regions editor.
	BioXASXASScanConfigurationRegionsEditor *regionsEditor_;
	/// The 'scientific detectors' view.
	AMGenericStepScanConfigurationDetectorsView *scientificDetectorsView_;
	/// The 'all detectors' view.
	AMGenericStepScanConfigurationDetectorsView *allDetectorsView_;
};

#endif // BIOXASXASSCANCONFIGURATIONEDITOR_H
