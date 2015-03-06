/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef VESPERSTIMESCANCONFIGURATIONVIEW_H
#define VESPERSTIMESCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "ui/VESPERS/VESPERSScanConfigurationView.h"
#include "acquaman/VESPERS/VESPERSTimeScanConfiguration.h"

#include <QLineEdit>
#include <QTextEdit>
#include <QButtonGroup>
#include <QDoubleSpinBox>
#include <QToolButton>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>

class VESPERSTimeScanConfigurationView : public VESPERSScanConfigurationView
{
	Q_OBJECT

public:
	VESPERSTimeScanConfigurationView(VESPERSTimeScanConfiguration *config, QWidget *parent = 0);

	/// Getter for the configuration.
	const AMScanConfiguration* configuration() const { return configuration_; }

protected slots:
	/// Helper slot that manages setting the time per point.
	void onDwellTimeChanged();
	/// Helper slot that manages setting the time per acquisition.
	void onTimePerAcquisitionChanged();
	/// Helper slot that manages setting the number of iterations.
	void onIterationsChanged();
	/// Handles passing the name of the CCD file name to the scan configuration when the CCD check box is enabled.
	void onCCDDetectorChanged(int id);
	/// Handles setting the name of the configuration from the line edit.
	void onScanNameEdited();
	/// Passes on the selection for I0 to the configuration.
	void onI0Clicked(int index) { configuration_->setIncomingChoice(index); }
	/// Handles changing what are acceptable choices for I0 based on It clicks.  Takes in the id of the new It choice.  Passes choice on to the configuration.
	void onItClicked(int index);
	/// Handles changes to the fluorescence detector choice.
	void onFluorescenceChoiceChanged(int id);
	/// Helper slot that handles the setting the estimated time label.
	void onEstimatedTimeChanged();
	/// Helper slot that sets whether we export spectra in rows or columns.
	void updateExportSpectraInRows(bool exportInColumns) { configuration_->setExportSpectraInRows(!exportInColumns); }

protected:
	/// Helper method that checks if the CCD files have the name given by \param name.  Does nothing if everything is okay.  Calls onCCDNameConflict if name conflicts exits.
	void checkCCDFileNames(const QString &name) const;

	/// Pointer to the specific scan config the view is modifying.
	VESPERSTimeScanConfiguration *configuration_;

	/// The time per acquisition box.
	QDoubleSpinBox *timePerAcquisitionBox_;
	/// The number of iterations box.
	QSpinBox *iterationsBox_;

	/// Pointer to the CCD help group box.
	QGroupBox *ccdTextBox_;
	/// Pointer to the label that holds the current map settings.
	QLabel *mapInfo_;
	/// Label holding the current estimated time for the scan to complete.  Takes into account extra time per point based on experience on the beamline.
	QLabel *estimatedTime_;
};

#endif // VESPERSTIMESCANCONFIGURATIONVIEW_H
