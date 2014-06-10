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


#ifndef CLSSIS3820SCALERSADETECTOR_H
#define CLSSIS3820SCALERSADETECTOR_H

#include "acquaman/AMSADetector.h"

class AMPVControl;

/*!
   BL1610-ID-2:mcs:delay = ___
   BL1610-ID-2:mcs:startScan
   BL1610-ID-2:mcs00:enable = 1
   BL1610-ID-2:mcs:continuous = 0

   read: BL1610-ID-2:mcs00:fbk


  */
/// Implements the AMSADetector interface for channels of an SIS3820 Scaler. This provides a 0D (single-valued) detector.
class CLSSIS3820ScalerSADetector : public AMSADetector
{
	Q_OBJECT
public:
	CLSSIS3820ScalerSADetector(const QString& name, const QString& description, const QString& pvBaseName, int channelNumber, bool sendTrigger = true, QObject* parent = 0, double initializeTimeoutSeconds = 3.0);
	virtual ~CLSSIS3820ScalerSADetector();

	virtual Type type() const { return Triggered; }


	/// Call to prepare the detector for the beginning of a scan. Returns false immediately if the detector cannot be used now. Will emit initialized() later once complete (or failed).
	/*! This version of Init() will wait for all the PVs to be connected, and then set the continuous mode to Off and the Enable for our channel to On. */
	virtual bool init();

	virtual double acquisitionTime() const;
	virtual bool setAcquisitionTime(double seconds);

	virtual bool isConnected() const;

	virtual bool initializationFinished() const { return initializationFinished_; }
	virtual bool initializationSucceeded() const { return initializationSucceeded_; }

	virtual bool isAcquiring() const { return isAcquiring_; }

	virtual bool lastAcquisitionFinished() const { return lastAcquisitionFinished_; }
	virtual bool lastAcquisitionSucceeded() const { return lastAcquisitionSucceeded_; }

	virtual bool acquireNow();
	virtual void cancelAcquisition();


	virtual AMNumber value(const AMnDIndex& index) const;
	virtual const double* data() const;


	int channelNumber() const { return channelNumber_; }

signals:

public slots:

protected slots:

	/// Whenever the feedback value changes. The scaler IOC software guarantees it will change whenever the acquisition is finished (even if the old valud is the same as the new one). We use this to detect acquistion complete + new value received.
	void onFeedbackValueChanged(double newValue);

	/// Called during initialization whenever enablePV_ and continuousPV_ change.
	void reviewInitialized();

	/// Called when the initialization times out (failed to connect, or enable and set mode by this timeout).
	void onInitializationTimeout();


	/// Called whenever the connection state of any of the controls changes.
	void onPVConnected();


protected:
	bool isAcquiring_;

	bool initializationFinished_;
	bool initializationSucceeded_;

	bool lastAcquisitionFinished_;
	bool lastAcquisitionSucceeded_;

	bool sendTrigger_;

	int channelNumber_;

	double data_;

	double initializeTimeoutSeconds_;

	bool wasConnected_;

	AMPVControl* delayPV_, *startPV_, *enablePV_, *continuousPV_, *feedbackPV_;
};

#endif // CLSSIS3820SCALERSADETECTOR_H
