#ifndef CLSPGTDWELLTIMECOORDINATOR_H
#define CLSPGTDWELLTIMECOORDINATOR_H

#include "beamline/AMControlSet.h"

class CLSPGTDwellTimeCoordinator : public QObject
{
Q_OBJECT
public:
	CLSPGTDwellTimeCoordinator(const QString &PGTStubName, const QString &beamlineStubName, QObject *parent = 0);

protected slots:
	void onRealTimeControlChanged(double realTime);
	void onLiveTimeControlChanged(double liveTime);
	void onPeakTimeControlChanged(double peakTime);
	void onDwellTimeControlChanged(double dwellTime);
	void onDwellTimeModeChanged(double dwellMode);
	void onAllControlsConnected(bool connected);

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

	AMControlSet *allControls_;
	bool connectedOnce_;

	bool internalRealTimeSet_;
	bool internalLiveTimeSet_;
	bool internalPeakTimeSet_;
	bool internalDwellTimeSet_;
	bool internalDwellModeSet_;
	double dwellTime_;
	int dwellMode_;
};

#endif // CLSPGTDWELLTIMECOORDINATOR_H
