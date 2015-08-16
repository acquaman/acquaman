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
#include "ui/acquaman/AMGenericStepScanConfigurationDetectorsView.h"
#include "ui/BioXAS/BioXASXASScanConfigurationEnergyEditor.h"
#include "ui/BioXAS/BioXASXASScanConfigurationRegionsEditor.h"

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
	void configurationChanged(BioXASXASScanConfiguration *newConfiguration);

public slots:
	/// Sets the XAS scan configuration being viewed.
	void setConfiguration(BioXASXASScanConfiguration *newConfiguration);

	/// Clears the view.
	void clear();
	/// Updates the view.
	void update();
	/// Refreshes the view.
	void refresh();

protected slots:
	/// Updates the displayed scan name to correspond to the configuration's scan name.
	void updateName();

	/// Updates the scan configuration name.
	void updateConfigurationName();

protected:
	/// The configuration being viewed.
	BioXASXASScanConfiguration *configuration_;

	/// Scan name editor.
    QLineEdit *scanName_;
	/// Energy editor.
	BioXASXASScanConfigurationEnergyEditor *energyEditor_;
	/// Regions editor.
	BioXASXASScanConfigurationRegionsEditor *regionsEditor_;
	/// The detectors view.
	AMGenericStepScanConfigurationDetectorsView *detectorsView_;
};

#endif // BIOXASXASSCANCONFIGURATIONVIEW_H
