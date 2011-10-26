#ifndef VESPERSEXPERIMENTCONFIGURATIONVIEW_H
#define VESPERSEXPERIMENTCONFIGURATIONVIEW_H

#include <QWidget>

#include <QButtonGroup>
#include <QAbstractButton>

#include "beamline/VESPERS/VESPERSExperimentConfiguration.h"
#include "util/VESPERS/VESPERSMapSetup.h"

/// This class is an interactive view that configures the beamline to a certain degree.  It's primary function is configuring the "Experiment Ready" flag in the beamline but also to auto-enable detectors in the synchronized dwell time.
class VESPERSExperimentConfigurationView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.  Builds a view around the experiment configuration model passed into it.
	explicit VESPERSExperimentConfigurationView(VESPERSExperimentConfiguration *experimentConfiguration, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Slot that handles the configuration changes.  Passes on the selection to the configuration model and also passes it along to the map setup model.
	void onConfiguraitonChanged(int id);
	/// Slot that sets the configurations to "custom" if any of the components are changed manually.
	void onComponentsChanged() { if (configurations_->checkedId() != 0) configurations_->button(0)->setChecked(true); }

protected:
	/// Pointer to the model that this view is showing.
	VESPERSExperimentConfiguration *experimentConfiguration_;
	/// Pointer to the model for the map launching model.
	VESPERSMapSetup *mapSetup_;

	/// The button group for the different types of setups.
	QButtonGroup *configurations_;
	/// The button group for the components that can be enabled.
	QButtonGroup *components_;
};

#endif // VESPERSEXPERIMENTCONFIGURATIONVIEW_H
