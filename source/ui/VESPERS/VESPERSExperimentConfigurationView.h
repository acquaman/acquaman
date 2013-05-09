/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/

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
	/// Slot that handles the sample stage choice changes.
	void onSampleStageChanged(int id);
	/// Updates the selected button based on changes to the experiment configuration for the sample stage.
	void onSampleStageUpdated(bool usingPseudoMotors);

protected:
	/// Pointer to the model that this view is showing.
	VESPERSExperimentConfiguration *experimentConfiguration_;
	/// Pointer to the model for the map launching model.
	VESPERSMapSetup *mapSetup_;

	/// The button group for the different types of setups.
	QButtonGroup *configurations_;
	/// The button group for the components that can be enabled.
	QButtonGroup *components_;
	/// The button group for the two sample stage choices.
	QButtonGroup *sampleStage_;

	/// Button used for resetting the pseudo motors.
	QPushButton *resetPseudoMotorsButton_;
};

#endif // VESPERSEXPERIMENTCONFIGURATIONVIEW_H
