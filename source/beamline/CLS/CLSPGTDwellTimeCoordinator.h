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


#ifndef CLSPGTDWELLTIMECOORDINATOR_H
#define CLSPGTDWELLTIMECOORDINATOR_H

#include "beamline/AMControlSet.h"

class CLSPGTDwellTimeCoordinator : public QObject
{
Q_OBJECT
public:
 	virtual ~CLSPGTDwellTimeCoordinator();
	CLSPGTDwellTimeCoordinator(const QString &PGTStubName, const QString &beamlineStubName, QObject *parent = 0);

protected slots:
	void onRealTimeControlChanged(double realTime);
	void onLiveTimeControlChanged(double liveTime);
	void onPeakTimeControlChanged(double peakTime);
	void onDwellTimeControlChanged(double dwellTime);
	void onDwellTimeModeChanged(double dwellMode);
	void onDwellTriggerControlChanged(double dwellTrigger);
	void onAllControlsConnected(bool connected);

	void onOldROI0Changed(double roi0);
	void onOldAcquiringControlChanged(double acquiring);

protected:
	void internalSetRealTime(double realTime);
	void internalSetLiveTime(double liveTime);
	void internalSetPeakTime(double peakTime);
	void internalSetDwellTime(double dwellTime);
	void internalSetDwellMode(int dwellMode);

	int determineMode() const;
	double determineTime() const;

protected:
	AMControl *realTimeControl_;
	AMControl *liveTimeControl_;
	AMControl *peakTimeControl_;
	AMControl *dwellTimeControl_;
	AMControl *dwellModeControl_;
	AMControl *dwellTriggerControl_;
	AMControl *oldTriggerControl_;
	AMControl *oldROI0Count_;
	AMControl *oldClearSpectrumControl_;
	AMControl *oldAcquiringControl_;
	AMControl *oldElapsedEnableControl_;

	AMControlSet *allControls_;
	bool connectedOnce_;

	bool internalRealTimeSet_;
	bool internalLiveTimeSet_;
	bool internalPeakTimeSet_;
	bool internalDwellTimeSet_;
	bool internalDwellModeSet_;
	double dwellTime_;
	int dwellMode_;

	bool internalDwellTrigger_;
	bool internalDwellTriggerIgnoreZero_;
};

#endif // CLSPGTDWELLTIMECOORDINATOR_H
