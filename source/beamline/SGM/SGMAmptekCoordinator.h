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


#ifndef SGMAMPTEKCOORDINATOR_H
#define SGMAMPTEKCOORDINATOR_H

#include "beamline/AMControlSet.h"

class SGMAmptekCoordinator : public QObject
{
Q_OBJECT
public:
	/// Simple constructor for class
 	virtual ~SGMAmptekCoordinator();
	SGMAmptekCoordinator(QObject *parent = 0);

protected slots:
	/// Handles watching for when the amptek controls actually connect
	void onAllAmptekControlsConnected(bool connected);

	/// Handles changes from the all:spectrum:startAcquisition control
	void onAmptekAllStartAcquisitionControlChanged(double startAcquisition);
	/// Handles changes from the all:spectrum:clear control
	void onAmptekAllSpectrumClearControlChanged(double spectrumClear);
	/// Handles changes from the all:parameters:PresetTime control
	void onAmptekAllPresetTimeControlChanged(double presetTime);

	/// Handles any changes to individual spectrum acquistion state controls to coordinate the master control
	void onAmptekSpectrumStateControlsChanged(double spectrumState);

protected:
	/// Control for coordinating all of the amptek start acquistions
	AMControl *AmptekAllStartAcquisitionControl_;
	/// Control for coordinating all of the amptek spectrum clears
	AMControl *AmptekAllSpectrumClearControl_;
	/// Control for coordinating all of the amptek preset times
	AMControl *AmptekAllPresetTimeControl_;
	/// Control for coordinating all of the amptek spectrum acquisition states
	AMControl *AmptekAllSpectrumStateControl_;

	/// Read only control for state of amptek sdd1 availability
	AMControl *AmptekSDD1IsAvailableControl_;
	/// Read only control for state of amptek sdd2 availability
	AMControl *AmptekSDD2IsAvailableControl_;
	/// Read only control for state of amptek sdd3 availability
	AMControl *AmptekSDD3IsAvailableControl_;
	/// Read only control for state of amptek sdd4 availability
	AMControl *AmptekSDD4IsAvailableControl_;
	/// Read only control for state of amptek sdd5 availability
	AMControl *AmptekSDD5IsAvailableControl_;
	/// Read only control for the state of amptek sdd1 requested status
	AMControl *AmptekSDD1IsRequestedControl_;
	/// Read only control for the state of amptek sdd2 requested status
	AMControl *AmptekSDD2IsRequestedControl_;
	/// Read only control for the state of amptek sdd3 requested status
	AMControl *AmptekSDD3IsRequestedControl_;
	/// Read only control for the state of amptek sdd4 requested status
	AMControl *AmptekSDD4IsRequestedControl_;
	/// Read only control for the state of amptek sdd5 requested status
	AMControl *AmptekSDD5IsRequestedControl_;
	/// Read only control for the spectrum acquisition state of sdd1
	AMControl *AmptekSDD1SpectrumStateControl_;
	/// Read only control for the spectrum acquisition state of sdd2
	AMControl *AmptekSDD2SpectrumStateControl_;
	/// Read only control for the spectrum acquisition state of sdd3
	AMControl *AmptekSDD3SpectrumStateControl_;
	/// Read only control for the spectrum acquisition state of sdd4
	AMControl *AmptekSDD4SpectrumStateControl_;
	/// Read only control for the spectrum acquisition state of sdd5
	AMControl *AmptekSDD5SpectrumStateControl_;

	/// Control for starting acquisition for amptek sdd1
	AMControl *AmptekSDD1StartAcquisitionControl_;
	/// Control for starting acquisition for amptek sdd2
	AMControl *AmptekSDD2StartAcquisitionControl_;
	/// Control for starting acquisition for amptek sdd3
	AMControl *AmptekSDD3StartAcquisitionControl_;
	/// Control for starting acquisition for amptek sdd4
	AMControl *AmptekSDD4StartAcquisitionControl_;
	/// Control for starting acquisition for amptek sdd5
	AMControl *AmptekSDD5StartAcquisitionControl_;
	/// Control for spectrum clearing for amptek sdd1
	AMControl *AmptekSDD1SpectrumClearControl_;
	/// Control for spectrum clearing for amptek sdd2
	AMControl *AmptekSDD2SpectrumClearControl_;
	/// Control for spectrum clearing for amptek sdd3
	AMControl *AmptekSDD3SpectrumClearControl_;
	/// Control for spectrum clearing for amptek sdd4
	AMControl *AmptekSDD4SpectrumClearControl_;
	/// Control for spectrum clearing for amptek sdd5
	AMControl *AmptekSDD5SpectrumClearControl_;
	/// Control for setting amptek SDD 1 preset time
	AMControl *AmptekSDD1PresetTimeControl_;
	/// Control for setting amptek SDD 2 preset time
	AMControl *AmptekSDD2PresetTimeControl_;
	/// Control for setting amptek SDD 3 preset time
	AMControl *AmptekSDD3PresetTimeControl_;
	/// Control for setting amptek SDD 4 preset time
	AMControl *AmptekSDD4PresetTimeControl_;
	/// Control for setting amptek SDD 5 preset time
	AMControl *AmptekSDD5PresetTimeControl_;

	/// All the amptek control (for checking connectivity)
	AMControlSet *allAmptekControls_;
	/// Holds whether or not the amptek controls connected at least once
	bool amptekConnectedOnce_;
};

#endif // SGMADDONSCOORDINATOR_H
