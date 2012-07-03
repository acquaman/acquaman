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


#ifndef CLSAMPTEKSDD123DETECTOR_H
#define CLSAMPTEKSDD123DETECTOR_H

#include "dataman/info/CLSAmptekSDD123DetectorInfo.h"
#include "beamline/AMDetector.h"

#include "beamline/AMControlSet.h"
#include "beamline/AMPVControl.h"
#include "dataman/datasource/AMProcessVariableDataSource.h"

/// This class represents a single Amptek SDD 123 Detector
class CLSAmptekSDD123Detector : public CLSAmptekSDD123DetectorInfo, public AMDetector
{
Q_OBJECT

public:
	/// Default constructor. Requires the name and base PV of the detector. It builds all the PV's and connects them accordingly.
	CLSAmptekSDD123Detector(const QString &name, const QString &baseName, AMDetector::ReadMethod readMethod = AMDetector::ImmediateRead, QObject *parent = 0);

	~CLSAmptekSDD123Detector();

	/// Return the meta object
	const QMetaObject *getMetaObject();

	/// Return the dacq compatible name for this detector
	virtual QString dacqName() const;

	/// Returns a string list of dacq "Action Move" statements. The strings should be divided into three sections by ||=||. Three sections are the command (SetPV, WaitPV, etc), the PV, and the value.
	virtual QStringList dacqMove() const;
	/// Returns a string list of dacq "Action Dwell" statements. The strings should be divided into three sections by ||=||. Three sections are the command (SetPV, WaitPV, etc), the PV, and the value.
	virtual QStringList dacqDwell() const;

	/// Returns the description for the detector.
	virtual QString description() const;

	/// Transforms current settings into a detector into. Returns a new instance-- caller is responsible for memory.
	virtual AMDetectorInfo *toInfo() const;
	/// Transforms current settings into a detector info.
	CLSAmptekSDD123DetectorInfo toCLSAmptekSDD123Info() const;
	/// Takes a detector info and sets all the settings for the detector.
	virtual bool setFromInfo(const AMDetectorInfo *info);
	/// Takes in a detector info and sets all the settings for the detector.
	void fromCLSAmptekSDD123Info(const CLSAmptekSDD123DetectorInfo &info);

	/// Holds whether the detector was connected previously.  Primarily useful at startup.
	bool wasConnected() const;

	// Getters that aren't included in the info.  These are convenience functions that grab the current value from the control.
	//////////////////////////////////////////////////

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

	virtual QDebug qDebugPrint(QDebug &d) const;

public slots:

	/// Erases the current spectrum and starts collecting data.
	void start();

	/*
	/// Set the accumulation time.
	void setIntegrationTime(double time);
	/// Sets the peaking time of the detector.
	void setPeakingTimeControl(double time);

	/// Sets the size of the spectra channels.
	void setChannelSize();
	*/
	/// Sets the description of the detector.
	void setDescription(const QString &description);


signals:
	/// Only emitted as true when all of the controls in the detector are connected. Is emitted false when any of the controls within the detector become unconnected.
	void connected(bool);
	/// This signal is emitted when the status changes.  Passes the state as a bool.  True is acquiring, false is done.
	void statusChanged(bool);
	/// Notifies that the peaking time has changed.
	void peakingTimeChanged(double);
	/// Notifies that the integration time has changed.
	void integrationTimeChanged(double);
	/// Notifies that the detector temperature has changed
	void detectorTemperatureChanged(double);
	/// Notifies that the number of MCA Channels has changed
	void mcaChannelsChanged(double);
	/// Notifies that the total counts in the spectrum has changed
	void totalCountsChanged(double);

protected slots:
	/// Determines if the detector is connected to ALL controls and process variables.
	void onControlsConnected(bool connected);
	/// Handles if one or more the controls times out
	void onControlsTimedOut();
	/// Emits the statusChanged signal.
	void onStatusChanged(double status);

protected:
	/// Bool handling whether the detector was connected.
	bool wasConnected_;

	/// The status control
	AMControl *statusControl_;
	/// The number of channels control
	AMControl *mcaChannelsControl_;
	/// The integration time control
	AMControl *integrationTimeControl_;
	/// The detector temperature control
	AMControl *detectorTemperatureControl_;
	/// The detector start control
	AMControl *startAcquisitionControl_;
	/// The detector spectrum control
	AMControl *spectrumControl_;
	/// A binned version of the detector spectrum control
	AMControl *binnedSpectrumControl_;

	/// The master set of controls
	AMControlSet *allControls_;

	/// The list of all the raw spectrum data sources.
	AM1DProcessVariableDataSource *spectrumDataSource_;

	QString baseName_;
};

#endif // CLSAMPTEKSDD123DETECTOR_H
