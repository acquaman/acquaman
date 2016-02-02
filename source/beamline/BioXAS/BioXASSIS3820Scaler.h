#ifndef BIOXASSIS3820SCALER_H
#define BIOXASSIS3820SCALER_H

#include <QObject>

#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/BioXAS/BioXASZebraSoftInputControl.h"

class AMControl;

class BioXASSIS3820Scaler : public CLSSIS3820Scaler
{
	Q_OBJECT

public:
	/// The constructor.
	BioXASSIS3820Scaler(const QString &baseName, BioXASZebraSoftInputControl *softInput, QObject *parent = 0);
	/// The destructor.
	virtual ~BioXASSIS3820Scaler();

	/// Returns true if the scaler is connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the inputs mode control.
	AMControl* inputsModeControl() const { return inputsMode_; }
	/// Returns the trigger source control.
	AMControl* triggerSourceModeControl() const { return triggerSourceMode_; }
	/// Returns the clock source mode control.
	AMControl* clockSourceModeControl() const { return clockSourceMode_; }

	/// The BioXAS scaler requires arming
	virtual bool requiresArming() { return true; }

	/// Creates and returns a new action that moves the scaler to 'Single shot' mode.
	virtual AMAction3* createMoveToSingleShotAction();
	/// Creates and returns a new action that moves the scaler to 'Continuous' mode.
	virtual AMAction3* createMoveToContinuousAction();

public slots:
	/// The BioXAS scaler requires arming
	virtual void arm();

	/// Our trigger source will need to be provided to us
	void setTriggerSource(AMDetectorTriggerSource *triggerSource);

protected slots:
	/// Handles listening to the start toggle for changes in the armed state
	void onStartToggleArmed();

	/// Actually handle triggering
	virtual void onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode readMode);
	/// Handles the logic for the removing channels from the waiting list.
	void onChannelReadingChanged(int channelIndex);
	/// Handles checking to see if we are done the acquisition.
	void triggerAcquisitionFinished();

protected:
	/// Controls the inputs mode.
	AMControl *inputsMode_;
	/// Controls the trigger source mode.
	AMControl *triggerSourceMode_;
	/// Controls the clock source mode.
	AMControl *clockSourceMode_;
	/// The control for the soft input control of the zebra.  This is the actual trigger now.
	BioXASZebraSoftInputControl *softInput_;

	/// Flags that arming is in process
	bool isArming_;
	bool isTriggered_;
};

#endif // BIOXASSIS3820SCALER_H
