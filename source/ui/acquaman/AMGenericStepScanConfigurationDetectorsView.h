#ifndef AMGENERICSTEPSCANCONFIGURATIONDETECTORSVIEW_H
#define AMGENERICSTEPSCANCONFIGURATIONDETECTORSVIEW_H

#include <QGroupBox>
#include <QButtonGroup>
#include <QLayout>
#include <QCheckBox>

#include "acquaman/AMGenericStepScanConfiguration.h"
#include "beamline/AMDetectorSet.h"

class AMGenericStepScanConfigurationDetectorsView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMGenericStepScanConfigurationDetectorsView(AMGenericStepScanConfiguration *configuration, AMDetectorSet *detectors, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMGenericStepScanConfigurationDetectorsView();

	/// Returns the configuration being viewed.
	AMScanConfiguration* configuration() const { return configuration_; }
	/// Returns the detector set being viewed.
	AMDetectorSet* detectors() const { return detectors_; }

signals:
	/// Notifier that the scan configuration being viewed has changed.
	void configurationChanged(AMGenericStepScanConfiguration *newConfiguration);
	/// Notifier that the detector set being viewed has changed.
	void detectorsChanged(AMDetectorSet *newDetectors);

public slots:
	/// Sets the scan configuration being viewed.
	void setConfiguration(AMGenericStepScanConfiguration *newConfiguration);
	/// Sets the detector set being viewed.
	void setDetectors(AMDetectorSet *newDetectors);

	/// Clears the view.
	void clear();
	/// Updates the view.
	void update();
	/// Refreshes the view.
	void refresh();

protected slots:
	/// Handles updating the configurations detector infos.
	void onDetectorSelectionChanged(int buttonIndex);

protected:
	/// The scan configuration being viewed.
	AMGenericStepScanConfiguration *configuration_;
	/// The detector set being viewed.
	AMDetectorSet *detectors_;

	/// The detectors button group.
	QButtonGroup *detectorsButtonGroup_;
	/// The main layout.
	QVBoxLayout *layout_;
};

#endif // AMGENERICSTEPSCANCONFIGURATIONDETECTORSVIEW_H
