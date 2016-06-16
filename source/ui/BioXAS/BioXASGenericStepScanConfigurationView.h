#ifndef BIOXASGENERICSTEPSCANCONFIGURATIONVIEW_H
#define BIOXASGENERICSTEPSCANCONFIGURATIONVIEW_H

#include <QWidget>
#include <QLineEdit>
#include <QTabWidget>

#include "beamline/AMControlSet.h"
#include "beamline/AMDetectorSet.h"
#include "ui/acquaman/AMScanConfigurationView.h"
#include "ui/BioXAS/BioXASGenericStepScanConfigurationAxesView.h"
#include "ui/acquaman/AMGenericStepScanConfigurationDetectorsView.h"

#define BIOXASGENERICSTEPSCANCONFIGURATIONVIEW_DETECTOR_VIEW_MIN_WIDTH 200

class BioXASGenericStepScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASGenericStepScanConfigurationView(AMGenericStepScanConfiguration *configuration, AMControlSet *controls, AMDetectorSet *detectors, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASGenericStepScanConfigurationView();

	/// Returns the configuration being viewed.
	AMGenericStepScanConfiguration* configuration() const { return configuration_; }
	/// Returns the set of control options.
	AMControlSet* controls() const { return controls_; }
	/// Returns the set of detector options.
	AMDetectorSet* detectors() const { return detectors_; }

signals:
	/// Notifier that the configuration being viewed has changed.
	void configurationChanged(AMGenericStepScanConfiguration *newConfiguration);
	/// Notifier that the set of control options have changed.
	void controlsChanged(AMControlSet *newControls);
	/// Notifier that the set of detector options have changed.
	void detectorsChanged(AMDetectorSet *newDetectors);

public slots:
	/// Sets the configuration being viewed.
	void setConfiguration(AMGenericStepScanConfiguration *newConfiguration);
	/// Sets the set of control options.
	void setControls(AMControlSet *newControls);
	/// Sets the set of detector options.
	void setDetectors(AMDetectorSet *newDetectors);

protected slots:
	/// Updates the name editor.
	void updateNameLineEdit();
	/// Handles updating the configuration with changes to the name editor.
	void onNameLineEditTextChanged();

	/// Updates the axes view.
	void updateAxesView();

	/// Updates the detectors view.
	void updateDetectorsView();

protected:
	/// The configuration being viewed.
	AMGenericStepScanConfiguration *configuration_;
	/// The set of control options.
	AMControlSet *controls_;
	/// The set of detector options.
	AMDetectorSet *detectors_;

	/// The scan name editor.
	QLineEdit *nameLineEdit_;
	/// The scan axes view.
	BioXASGenericStepScanConfigurationAxesView *axesView_;
	/// The detectors view.
	AMGenericStepScanConfigurationDetectorsView *detectorsView_;
};

#endif // BIOXASGENERICSTEPSCANCONFIGURATIONVIEW_H
