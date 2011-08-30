#ifndef VESPERSEXPERIMENTCONFIGURATIONVIEW_H
#define VESPERSEXPERIMENTCONFIGURATIONVIEW_H

#include <QWidget>

#include <QButtonGroup>
#include <QAbstractButton>

/// This class is an interactive view that configures the beamline to a certain degree.  It's primary function is configuring the "Experiment Ready" flag in the beamline but also to auto-enable detectors in the synchronized dwell time.
class VESPERSExperimentConfigurationView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.
	explicit VESPERSExperimentConfigurationView(QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Slot that handles which components will be set.
	void onConfigurationsChanged(int id);
	/// Slot that sets the configurations to "custom" if any of the components are changed manually.
	void onComponentsChanged() { configurations_->button(0)->setChecked(true); }

protected:
	/// The button group for the different types of setups.
	QButtonGroup *configurations_;
	/// The button group for the components that can be enabled.
	QButtonGroup *components_;
};

#endif // VESPERSEXPERIMENTCONFIGURATIONVIEW_H
