#ifndef BIOXASSIS3820SCALER_H
#define BIOXASSIS3820SCALER_H

#include <QObject>

#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/BioXAS/BioXASZebraSoftInputControl.h"

#define BIOXASSIS3820SCALER_WAIT_SECONDS 0.5

class AMControl;

class BioXASSIS3820Scaler : public CLSSIS3820Scaler
{
	Q_OBJECT

public:
	/// Enum describing the possible armed states.
	enum ArmedMode { NotArmed = 0, Armed = 1 };
	/// Enum describing the inputs mode states.
	enum InputsMode { Mode0 = 0, Mode1 = 1 };
	/// Enum describing the trigger source mode states.
	enum TriggerSourceMode { Software = 0, Hardware = 1 };

	/// The constructor.
	BioXASSIS3820Scaler(const QString &baseName, BioXASZebraSoftInputControl *softInput, QObject *parent = 0);
	/// The destructor.
	virtual ~BioXASSIS3820Scaler();

	/// Returns true if the scaler is connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns whether the scaler is currently armed.
	bool isArmed() const;
	/// Returns whether the scaler is currently scanning.
	virtual bool isScanning() const { return scanning_; }

	/// Returns the inputs mode control.
	AMControl* inputsModeControl() const { return inputsMode_; }
	/// Returns the trigger source control.
	AMControl* triggerSourceModeControl() const { return triggerSourceMode_; }

	/// The BioXAS scaler requires arming
	virtual bool requiresArming() const { return true; }

	/// Creates an action that arms or disarms the scaler.
	AMAction3* createArmAction(bool setArmed);
	/// Creates and returns a new action that moves the scaler to 'Armed' mode.
	AMAction3* createMoveToArmedAction();
	/// Creates and returna a new action that moves the scaler to 'NotArmed' mode.
	AMAction3* createMoveToNotArmedAction();

	/// Creates an action that starts the scaler scanning. Reimplemented to include arming the scaler.
	virtual AMAction3* createStartAction3(bool setScanning);

	/// Creates and returns a new action that moves the scaler to 'Scanning' mode.
	virtual AMAction3* createMoveToScanningAction();
	/// Creates and returns a new action that moves the scaler to 'NotScanning' mode.
	virtual AMAction3* createMoveToNotScanningAction();

	/// Creates and returns a new action that moves the scaler to 'Single shot' mode.
	virtual AMAction3* createMoveToSingleShotAction();
	/// Creates and returns a new action that moves the scaler to 'Continuous' mode.
	virtual AMAction3* createMoveToContinuousAction();

	/// Creates a new action that causes this scaler to take a dark current measurement.
	virtual AMAction3* createMeasureDarkCurrentAction(double secondsDwell);

signals:
	/// Notifier that the inputs mode value preference has changed.
	void inputsModeValuePreferenceChanged(double newValue);
	/// Notifier that the trigger source mode value preference has changed.
	void triggerSourceModeValuePreferenceChanged(double newValue);

public slots:
	/// The BioXAS scaler requires arming
	virtual void arm();

	/// Our trigger source will need to be provided to us
	void setTriggerSource(AMDetectorTriggerSource *triggerSource);

	/// Sets the inputs mode value preference.
	void setInputsModeValuePreference(double newValue);
	/// Sets the trigger source mode value preference.
	void setTriggerSourceModeValuePreference(double newValue);

protected slots:
	/// Sets the scaler's scanning state.
	void setScanningState(bool isScanning);

	/// Handles listening to the start toggle for changes in the armed state
	void onStartToggleArmed();
	/// Handles updating the scanning state in response to changes in the soft input value.
	void onSoftInputValueChanged();
	/// Handles updating the scanning state in response to changes in the startToggle_ value.
	void onStartToggleValueChanged();

	/// Actually handle triggering
	virtual void onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode readMode);
	/// Handles reporting a trigger as succeeded when all scaler channels have reported a new value. Reimplemented to additionally check that the scanning state is accurate.
	virtual void onReadingChanged(double value);

	/// Handles applying the inputs mode value preference, if a preference has been set.
	void updateInputsModeControl();
	/// Handles applying the trigger source mode value preference, if a preference has been set.
	void updateTriggerSourceModeControl();

protected:
	/// Method that calls set succeeded on the trigger source.  Reimplemented to use setSucceeded from zebra trigger source.
	virtual void triggerSourceSucceeded();

protected:
	/// The scanning state.
	bool scanning_;

	/// Controls the inputs mode.
	AMControl *inputsMode_;
	/// Flag indicating whether an inputs mode preference has been set.
	bool inputsModeValuePreferenceSet_;
	/// The inputs mode value preference.
	double inputsModeValuePreference_;

	/// Controls the trigger source mode.
	AMControl *triggerSourceMode_;
	/// Flag indicating whether a trigger source mode preference has been set.
	bool triggerSourceModeValuePreferenceSet_;
	/// The trigger source mode value preference.
	double triggerSourceModeValuePreference_;

	/// The control for the soft input control of the zebra.  This is the actual trigger now.
	BioXASZebraSoftInputControl *softInput_;

	/// Flag for the arming process
	bool isArming_;
};

#endif // BIOXASSIS3820SCALER_H
