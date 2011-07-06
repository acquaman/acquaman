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


#ifndef PGTDETECTOR_H
#define PGTDETECTOR_H

#include "AMDetector.h"
#include "dataman/PGTDetectorInfo.h"
#include "AMControlSet.h"

class PGTDetector : public PGTDetectorInfo, public AMDetector
{
	Q_OBJECT
public:
	PGTDetector(const QString &name, AMControlSet *readingsControls, AMControlSet *settingsControls, AMBeamlineActionItem *toggleOnAction, AMBeamlineActionItem *toggleOffAction, AMDetector::ReadMethod readMethod = AMDetector::ImmediateRead, QObject *parent = 0);
	PGTDetector(const QString& name, AMControl *dataWaveform, AMControl *hv, AMControl *integrationTime, AMControl *integrationMode, AMBeamlineActionItem *toggleOnAction, AMBeamlineActionItem *toggleOffAction, AMDetector::ReadMethod readMethod = AMDetector::ImmediateRead, QObject *parent = 0);
	~PGTDetector();

	const QMetaObject* getMetaObject();

	virtual double reading() const;

	/// NEEDS TO RETURN A NEW INSTANCE, CALLER IS RESPONSIBLE FOR MEMORY.
	AMDetectorInfo* toInfo() const;
	PGTDetectorInfo toPGTInfo() const;

	/* NTBA March 14, 2011 David Chevrier
	bool setFromInfo(const AMDetectorInfo &info);
	*/
	bool setFromInfo(const AMDetectorInfo *info);
	bool setFromInfo(const PGTDetectorInfo &info);

	bool isPoweredOn();

	bool activate();
	AMBeamlineActionItem* turnOnAction();

	AMControl* dataWaveformCtrl() const;
	AMControl* hvCtrl() const;
	AMControl* integrationTimeCtrl() const;
	AMControl* integrationModeCtrl() const;

	bool settingsMatchFbk(PGTDetectorInfo* settings);

	QString description() const;

public slots:
	void setDescription(const QString &description);
	virtual bool setControls(PGTDetectorInfo *pgtSettings);

signals:
	void poweredOnChanged(bool poweredOn);

protected slots:
	void onControlsConnected(bool connected);
	void onReadingsControlValuesChanged();
	void onSettingsControlValuesChanged();

protected:
	bool initializeFromControlSet(AMControlSet *readingsControls, AMControlSet *settingsControls);

protected:
	AMControlSet *readingsControls_;
	AMControlSet *settingsControls_;
	bool ownsControlSets_;

	AMBeamlineActionItem *toggleOnAction_;
	AMBeamlineActionItem *toggleOffAction_;

private:
	bool poweredOn_;
};


#endif // PGTDETECTOR_H
