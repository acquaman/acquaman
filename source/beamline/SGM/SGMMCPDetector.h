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


#ifndef SGMMCPDETECTOR_H
#define SGMMCPDETECTOR_H

#include "beamline/AMOldDetector.h"
#include "dataman/SGM/SGMMCPDetectorInfo.h"
#include "beamline/AMControlSet.h"
#include "beamline/AMPVControl.h"
#include "actions/AMBeamlineActionItem.h"

class SGMMCPDetector : public SGMMCPDetectorInfo, public AMOldDetector
{
	Q_OBJECT
public:
	SGMMCPDetector(const QString &name, const QString &fullReadingName, const QString &hvBaseName, AMBeamlineActionItem *toggleOnAction, AMBeamlineActionItem *toggleOffAction, AMOldDetector::ReadMethod readMethod = AMOldDetector::ImmediateRead, QObject *parent = 0);
	~SGMMCPDetector();

	const QMetaObject* getMetaObject();

	virtual QString dacqName() const;

	virtual double reading() const;

	/// NEEDS TO RETURN A NEW INSTANCE, CALLER IS RESPONSIBLE FOR MEMORY.
	AMOldDetectorInfo* toInfo() const;
	SGMMCPDetectorInfo toMCPInfo() const;

	bool isPoweredOn();

	AMControl* readingCtrl() const;
	AMControl* hvCtrl() const;

	/* NTBA March 14, 2011 David Chevrier
	bool setFromInfo(const AMDetectorInfo &info);
	   */
	bool setFromInfo(const AMOldDetectorInfo *info);
	bool setFromInfo(const SGMMCPDetectorInfo &info);

	bool activate();
	AMBeamlineActionItem* turnOnAction();

	bool settingsMatchFbk(SGMMCPDetectorInfo* settings);

	QString description() const;

public slots:
	void setDescription(const QString &description);
	virtual bool setControls(SGMMCPDetectorInfo *mcpSettings);

	void onDetectorHVToggleChanged();

signals:
	void poweredOnChanged(bool poweredOn);
	void detectorHVChanged();

protected slots:
	void onControlsConnected(bool connected);
	/// Handles if one or more the controls times out
	void onControlsTimedOut();
	void onReadingsControlValuesChanged();
	void onSettingsControlValuesChanged();

protected:
	/// The actual control for the reading value
	AMControl *readingControl_;
	/// The control for the MCP High voltage
	AMControl *hvControl_;

	/// A control set for all the controls (for ease of signalling)
	AMControlSet *allControls_;

	AMBeamlineActionItem *toggleOnAction_;
	AMBeamlineActionItem *toggleOffAction_;

private:
	bool poweredOn_;
};


#endif // SGMMCPDETECTOR_H
