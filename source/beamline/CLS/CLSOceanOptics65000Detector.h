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


#ifndef CLSOCEANOPTICS65000DETECTOR_H
#define CLSOCEANOPTICS65000DETECTOR_H

#include "beamline/AMOldDetector.h"
#include "dataman/info/CLSOceanOptics65000DetectorInfo.h"
#include "beamline/AMControlSet.h"
#include "beamline/AMPVControl.h"
#include "dataman/datasource/AMProcessVariableDataSource.h"

class CLSOceanOptics65000Detector : public CLSOceanOptics65000DetectorInfo, public AMOldDetector
{
Q_OBJECT
public:
	/// Default constructor takes a base name for generation
	CLSOceanOptics65000Detector(const QString &name, const QString &baseName, AMOldDetector::ReadMethod readMethod = AMOldDetector::ImmediateRead, QObject *parent = 0);
	/// Destructor
	~CLSOceanOptics65000Detector();

	/// Returns the meta object
	const QMetaObject* getMetaObject();

	/// Returns the PV name the dacq library wants to use
	virtual QString dacqName() const;

	/// Returns a string list of dacq "Action Dwell" statements. The strings should be divided into three sections by ||=||. Three sections are the command (SetPV, WaitPV, etc), the PV, and the value.
	virtual QStringList dacqDwell() const;

	/// General reading value (in this case, the total counts in all bins)
	virtual double reading() const;

	/// NEEDS TO RETURN A NEW INSTANCE, CALLER IS RESPONSIBLE FOR MEMORY.
	/// Converts to a new instance of general detector info (snapshot of current state)
	AMOldDetectorInfo* toInfo() const;
	/// Converts to a by value copy of specific detector info type (snapshot of current state)
	CLSOceanOptics65000DetectorInfo toOceanOptics65000Info() const;

	/// Sets current state from a pointer to specific detector info type
	bool setFromInfo(const AMOldDetectorInfo *info);
	/// Sets current state from a reference to specific detector info type
	bool setFromInfo(const CLSOceanOptics65000DetectorInfo &info);

	/// Returns pointer to the data waveform control
	AMControl* dataWaveformControl() const;
	/// Returns pointer to the integration time contorl
	AMControl* integrationTimeControl() const;

	/// Checks the settings passed in to confirm they match the current values within tolerance
	bool settingsMatchFbk(CLSOceanOptics65000DetectorInfo *settings);

	/// Returns the description
	QString description() const;

	/// Returns the status as an bool.  true is acquiring, false is done.
	bool status() const;

	// End of getters that aren't included in the info.
	/////////////////////////////////////////////////////

	/// Turns the spectra controls into an array of ints and returns the spectra
	QVector<int> spectraValues();

	/// Returns the total spectra counts using the binned version
	int spectraTotalCounts();

	// Data sources
	///////////////////////////////////////
	/// Returns the raw spectrum data source at \c index.  It is assumed that the data sources will be in order of element.  Must be between 0 and size()-1.
	AMDataSource* spectrumDataSource() const;

public slots:
	/// Sets the description for the detector
	void setDescription(const QString &description);
	/// Sets the detector contorls to the state passed in
	bool setControls(CLSOceanOptics65000DetectorInfo *settings);

	/// Erases the current spectrum and starts collecting data.
	void start();

signals:
	/// Only emitted as true when all of the controls in the detector are connected. Is emitted false when any of the controls within the detector become unconnected.
	void connected(bool);
	/// This signal is emitted when the status changes.  Passes the state as a bool.  True is acquiring, false is done.
	void statusChanged(bool);
	/// Notifies that the total counts in the spectrum has changed
	void totalCountsChanged(double);

protected slots:
	/// Handles when controls change their connected state
	void onControlsConnected(bool connected);
	/// Handles if one or more the controls times out
	void onControlsTimedOut();
	/// Handles when the values for the reading controls change
	void onReadingsControlValuesChanged();
	/// Handles when the values for the settings controls change
	void onSettingsControlValuesChanged();

	/// Emits the statusChanged signal.
	void onStatusChanged(double status);

protected:
	/// The actual control for the spectrum waveform
	AMControl *dataWaveformControl_;
	/// The control for the integration time
	AMControl *integrationTimeControl_;
	/// The control for triggering the detector
	AMControl *dwellTriggerControl_;
	/// The control that reports the acquisition state
	AMControl *dwellStateControl_;

	/// A control set for all the controls (for ease of signalling)
	AMControlSet *allControls_;

	/// The list of all the raw spectrum data sources.
	AM1DProcessVariableDataSource *spectrumDataSource_;

	QString baseName_;
};

#endif // CLSOCEANOPTICS65000DETECTOR_H
