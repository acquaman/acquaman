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


#ifndef SGMMCPDETECTOR_H
#define SGMMCPDETECTOR_H

#include "beamline/AMDetector.h"
#include "dataman/SGM/SGMMCPDetectorInfo.h"
#include "beamline/AMControlSet.h"
#include "actions/AMBeamlineActionItem.h"

class SGMMCPDetector : public SGMMCPDetectorInfo, public AMDetector
{
	Q_OBJECT
public:
	SGMMCPDetector(const QString &name, AMControlSet *readingsControls, AMControlSet *settingsControls, AMBeamlineActionItem *toggleOnAction, AMBeamlineActionItem *toggleOffAction, AMDetector::ReadMethod readMethod = AMDetector::ImmediateRead, QObject *parent = 0);
	SGMMCPDetector(const QString& name, AMControl *reading, AMControl *hv, AMBeamlineActionItem *toggleOnAction, AMBeamlineActionItem *toggleOffAction, AMDetector::ReadMethod readMethod = AMDetector::ImmediateRead, QObject *parent = 0);
	~SGMMCPDetector();

	const QMetaObject* getMetaObject();

	virtual double reading() const;

	/// NEEDS TO RETURN A NEW INSTANCE, CALLER IS RESPONSIBLE FOR MEMORY.
	AMDetectorInfo* toInfo() const;
	SGMMCPDetectorInfo toMCPInfo() const;

	bool isPoweredOn();

	AMControl* readingCtrl() const;
	AMControl* hvCtrl() const;

	/* NTBA March 14, 2011 David Chevrier
	bool setFromInfo(const AMDetectorInfo &info);
	   */
	bool setFromInfo(const AMDetectorInfo *info);
	bool setFromInfo(const SGMMCPDetectorInfo &info);

	bool activate();
	AMBeamlineActionItem* turnOnAction();

	bool settingsMatchFbk(SGMMCPDetectorInfo* settings);

	QString description() const;

public slots:
	void setDescription(const QString &description);
	virtual bool setControls(SGMMCPDetectorInfo *mcpSettings);

signals:
	void poweredOnChanged(bool poweredOn);

protected slots:
	void onControlsConnected(bool connected);
	void onReadingsControlValuesChanged();
	void onSettingsControlValuesChanged();

protected:
	void initializeFromControlSet(AMControlSet *readingsControls, AMControlSet *settingsControls);

protected:
	AMControlSet *readingsControls_;
	AMControlSet *settingsControls_;
	bool ownsControlSets_;

	AMBeamlineActionItem *toggleOnAction_;
	AMBeamlineActionItem *toggleOffAction_;

private:
	bool poweredOn_;
};


#endif // SGMMCPDETECTOR_H
