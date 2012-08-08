#ifndef SGMAMPTEKCOORDINATOR_H
#define SGMAMPTEKCOORDINATOR_H

#include "beamline/AMControlSet.h"

class SGMAmptekCoordinator : public QObject
{
Q_OBJECT
public:
	/// Simple constructor for class
	SGMAmptekCoordinator(QObject *parent = 0);

protected slots:
	/// Handles watching for when the amptek controls actually connect
	void onAllAmptekControlsConnected(bool connected);

	/// Handles changes from the all:initialize control
	void onAmptekAllInitializeControlChanged(double initialize);
	/// Handles changes from the all:spectrum:startAcquisition control
	void onAmptekAllStartAcquisitionControlChanged(double startAcquisition);
	/// Handles changes from the all:spectrum:clear control
	void onAmptekAllSpectrumClearControlChanged(double spectrumClear);
	/// Handles changes from the all:parameters:PresetTime control
	void onAmptekAllPresetTimeControlChanged(double presetTime);

protected:
	/// Control for coordinating all of the amptek initializations
	AMControl *AmptekAllInitializeControl_;
	/// Control for coordinating all of the amptek start acquistions
	AMControl *AmptekAllStartAcquisitionControl_;
	/// Control for coordinating all of the amptek spectrum clears
	AMControl *AmptekAllSpectrumClearControl_;
	/// Control for coordinating all of the amptek preset times
	AMControl *AmptekAllPresetTimeControl_;
	/// Read only control for state of amptek sdd1 availability
	AMControl *AmptekSDD1IsAvailableControl_;
	/// Read only control for state of amptek sdd2 availability
	AMControl *AmptekSDD2IsAvailableControl_;
	/// Read only control for state of amptek sdd3 availability
	AMControl *AmptekSDD3IsAvailableControl_;
	/// Read only control for state of amptek sdd4 availability
	AMControl *AmptekSDD4IsAvailableControl_;
	/// Control for initializing amptek sdd1
	AMControl *AmptekSDD1InitializeControl_;
	/// Control for initializing amptek sdd2
	AMControl *AmptekSDD2InitializeControl_;
	/// Control for initializing amptek sdd3
	AMControl *AmptekSDD3InitializeControl_;
	/// Control for initializing amptek sdd4
	AMControl *AmptekSDD4InitializeControl_;
	/// Control for starting acquisition for amptek sdd1
	AMControl *AmptekSDD1StartAcquisitionControl_;
	/// Control for starting acquisition for amptek sdd2
	AMControl *AmptekSDD2StartAcquisitionControl_;
	/// Control for starting acquisition for amptek sdd3
	AMControl *AmptekSDD3StartAcquisitionControl_;
	/// Control for starting acquisition for amptek sdd4
	AMControl *AmptekSDD4StartAcquisitionControl_;
	/// Control for spectrum clearing for amptek sdd1
	AMControl *AmptekSDD1SpectrumClearControl_;
	/// Control for spectrum clearing for amptek sdd2
	AMControl *AmptekSDD2SpectrumClearControl_;
	/// Control for spectrum clearing for amptek sdd3
	AMControl *AmptekSDD3SpectrumClearControl_;
	/// Control for spectrum clearing for amptek sdd4
	AMControl *AmptekSDD4SpectrumClearControl_;
	/// Control for setting amptek SDD 1 preset time
	AMControl *AmptekSDD1PresetTimeControl_;
	/// Control for setting amptek SDD 2 preset time
	AMControl *AmptekSDD2PresetTimeControl_;
	/// Control for setting amptek SDD 3 preset time
	AMControl *AmptekSDD3PresetTimeControl_;
	/// Control for setting amptek SDD 4 preset time
	AMControl *AmptekSDD4PresetTimeControl_;

	/// All the amptek control (for checking connectivity)
	AMControlSet *allAmptekControls_;
	/// Holds whether or not the amptek controls connected at least once
	bool amptekConnectedOnce_;
};

#endif // SGMADDONSCOORDINATOR_H
