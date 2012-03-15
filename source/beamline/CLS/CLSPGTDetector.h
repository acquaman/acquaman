/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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

#include "beamline/AMDetector.h"
#include "dataman/info/CLSPGTDetectorInfo.h"
#include "beamline/AMControlSet.h"
#include "beamline/AMPVControl.h"

class CLSPGTDetector : public CLSPGTDetectorInfo, public AMDetector
{
	Q_OBJECT
public:
	CLSPGTDetector(const QString& name, const QString &baseName, AMBeamlineActionItem *toggleOnAction, AMBeamlineActionItem *toggleOffAction, AMDetector::ReadMethod readMethod = AMDetector::ImmediateRead, QObject *parent = 0);
	~CLSPGTDetector();

	const QMetaObject* getMetaObject();

	virtual QString dacqName() const;

	virtual double reading() const;

	/// NEEDS TO RETURN A NEW INSTANCE, CALLER IS RESPONSIBLE FOR MEMORY.
	AMDetectorInfo* toInfo() const;
	CLSPGTDetectorInfo toPGTInfo() const;

	/* NTBA March 14, 2011 David Chevrier
	bool setFromInfo(const AMDetectorInfo &info);
	*/
	bool setFromInfo(const AMDetectorInfo *info);
	bool setFromInfo(const CLSPGTDetectorInfo &info);

	bool isPoweredOn();

	bool activate();
	AMBeamlineActionItem* turnOnAction();

	AMControl* dataWaveformCtrl() const;
	AMControl* hvCtrl() const;
	AMControl* integrationTimeCtrl() const;
	AMControl* integrationModeCtrl() const;

	bool settingsMatchFbk(CLSPGTDetectorInfo* settings);

	QString description() const;

public slots:
	void setDescription(const QString &description);
	virtual bool setControls(CLSPGTDetectorInfo *pgtSettings);

signals:
	void poweredOnChanged(bool poweredOn);

protected slots:
	void onControlsConnected(bool connected);
	void onReadingsControlValuesChanged();
	void onSettingsControlValuesChanged();

protected:
	/// The actual control for the spectrum waveform
	AMControl *dataWaveformControl_;
	/// The control for the PGT high voltage
	AMControl *hvControl_;
	/// The control for the integration time
	AMControl *integrationTimeControl_;
	/// The control for the integration mode
	AMControl *integrationModeControl_;

	/// A control set for all the controls (for ease of signalling)
	AMControlSet *allControls_;

	/// The action for toggling the HV on (right now comes from somewhere else)
	AMBeamlineActionItem *toggleOnAction_;
	/// The action for toggling the HV off (right now comes from somewhere else)
	AMBeamlineActionItem *toggleOffAction_;

private:
	bool poweredOn_;
};


#endif // CLSPGTDETECTOR_H
