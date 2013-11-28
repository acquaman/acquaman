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


#ifndef CLSPGTDETECTOR_H
#define CLSPGTDETECTOR_H

//#include "beamline/AMOldDetector.h"
//#include "dataman/info/CLSPGTDetectorInfo.h"
//#include "beamline/AMControlSet.h"
//#include "beamline/AMPVControl.h"
//#include "dataman/datasource/AMProcessVariableDataSource.h"

//class CLSPGTDetector : public CLSPGTDetectorInfo, public AMOldDetector
//{
//	Q_OBJECT
//public:
//	CLSPGTDetector(const QString& name, const QString &baseName, AMBeamlineActionItem *toggleOnAction, AMBeamlineActionItem *toggleOffAction, AMOldDetector::ReadMethod readMethod = AMOldDetector::ImmediateRead, QObject *parent = 0);
//	~CLSPGTDetector();

//	const QMetaObject* getMetaObject();

//	/// Returns the PV name the dacq library wants to use
//	virtual QString dacqName() const;

//	/// Returns a string list of dacq "Action Begin" statements. The strings should be divided into three sections by ||=||. Three sections are the command (SetPV, WaitPV, etc), the PV, and the value.
//	virtual QStringList dacqBegin() const;
//	/// Returns a string list of dacq "Action Move" statements. The strings should be divided into three sections by ||=||. Three sections are the command (SetPV, WaitPV, etc), the PV, and the value.
//	virtual QStringList dacqMove() const;
//	/// Returns a string list of dacq "Action Dwell" statements. The strings should be divided into three sections by ||=||. Three sections are the command (SetPV, WaitPV, etc), the PV, and the value.
//	virtual QStringList dacqDwell() const;
//	/// Returns a string list of dacq "Action Finish" statements. The strings should be divided into three sections by ||=||. Three sections are the command (SetPV, WaitPV, etc), the PV, and the value.
//	virtual QStringList dacqFinish() const;

//	virtual double reading() const;

//	/// NEEDS TO RETURN A NEW INSTANCE, CALLER IS RESPONSIBLE FOR MEMORY.
//	AMOldDetectorInfo* toInfo() const;
//	CLSPGTDetectorInfo toPGTInfo() const;

//	/* NTBA March 14, 2011 David Chevrier
//	bool setFromInfo(const AMDetectorInfo &info);
//	*/
//	bool setFromInfo(const AMOldDetectorInfo *info);
//	bool setFromInfo(const CLSPGTDetectorInfo &info);

//	/// Returns the status as an bool.  true is acquiring, false is done.
//	bool status() const;

//	bool isPoweredOn();

//	bool activate();
//	AMBeamlineActionItem* turnOnAction();

//	AMControl* dataWaveformCtrl() const;
//	AMControl* hvCtrl() const;
//	AMControl* integrationTimeCtrl() const;
//	AMControl* integrationModeCtrl() const;
//	AMControl* dwellTriggerControl() const;

//	bool settingsMatchFbk(CLSPGTDetectorInfo* settings);

//	QString description() const;

//	/// Turns the spectra controls into an array of ints and returns the spectra
//	QVector<int> spectraValues();

//	/// Returns the total spectra counts using the binned version
//	int spectraTotalCounts();

//	/// Returns the raw spectrum data source at \c index.  It is assumed that the data sources will be in order of element.  Must be between 0 and size()-1.
//	AMDataSource* spectrumDataSource() const;

//public slots:
//	void setDescription(const QString &description);
//	virtual bool setControls(CLSPGTDetectorInfo *pgtSettings);

//	/// Erases the current spectrum and starts collecting data.
//	void start();

//signals:
//	/// Only emitted as true when all of the controls in the detector are connected. Is emitted false when any of the controls within the detector become unconnected.
//	void connected(bool);
//	void poweredOnChanged(bool poweredOn);
//	void statusChanged(bool);
//	/// Notifies that the total counts in the spectrum has changed
//	void totalCountsChanged(double);

//protected slots:
//	void onControlsConnected(bool connected);
//	/// Handles if one or more the controls times out
//	void onControlsTimedOut();
//	void onReadingsControlValuesChanged();
//	void onSettingsControlValuesChanged();
//	void onDwellTriggerChanged(double value);

//protected:
//	/// The actual control for the spectrum waveform
//	AMControl *dataWaveformControl_;
//	/// The control for the PGT high voltage
//	AMControl *hvControl_;
//	/// The control for the integration time
//	AMControl *integrationTimeControl_;
//	/// The control for the integration mode
//	AMControl *integrationModeControl_;
//	/// The control for triggering the detector
//	AMControl *dwellTriggerControl_;

//	/// A control set for all the controls (for ease of signalling)
//	AMControlSet *allControls_;

//	/// The action for toggling the HV on (right now comes from somewhere else)
//	AMBeamlineActionItem *toggleOnAction_;
//	/// The action for toggling the HV off (right now comes from somewhere else)
//	AMBeamlineActionItem *toggleOffAction_;

//	/// The list of all the raw spectrum data sources.
//	AM1DProcessVariableDataSource *spectrumDataSource_;

//protected:
//	QString baseName_;

//private:
//	bool poweredOn_;
//};


#endif // CLSPGTDETECTOR_H
