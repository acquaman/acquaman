#ifndef BIOXASSSRLMONOCHROMATORMASK_H
#define BIOXASSSRLMONOCHROMATORMASK_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"

class BioXASSSRLMonochromatorMaskState;

class BioXASSSRLMonochromatorMask : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Enumeration of different state values.
	enum State { Open = 0, Closed = 1 };

	/// Constructor.
	explicit BioXASSSRLMonochromatorMask(const QString &deviceName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorMask();

	/// Returns true if the mask is connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the upper blade control.
	AMControl* upperBlade() const { return upperBlade_; }
	/// Returns the lower blade control.
	AMControl* lowerBlade() const { return lowerBlade_; }
	/// Returns the blades state control.
	AMControl* bladesState() const { return bladesState_; }
	/// Returns the mask state control.
	BioXASSSRLMonochromatorMaskState* state() const { return state_; }

signals:
	/// Notifier that the upper slit blade control has changed.
	void upperBladeChanged(AMControl *newControl);
	/// Notifier that the lower slit blade control has changed.
	void lowerBladeChanged(AMControl *newControl);
	/// Notifier that the blades state control has changed.
	void bladesStateChanged(AMControl *newControl);
	/// Notifier that the mask state control has changed.
	void stateChanged(BioXASSSRLMonochromatorMaskState *newControl);

public slots:
	/// Sets the upper blade control.
	void setUpperBlade(AMControl *newControl);
	/// Sets the lower blade control.
	void setLowerBlade(AMControl *newControl);
	/// Sets the blades state control.
	void setBladesState(AMControl *newControl);
	/// Sets the mask state control.
	void setState(BioXASSSRLMonochromatorMaskState *newControl);

protected slots:
	/// Updates the state control with the upper blade, lower blade, and state PV controls.
	void updateState();

protected:
	/// The upper blade control.
	AMControl *upperBlade_;
	/// The lower blade control.
	AMControl *lowerBlade_;
	/// The state PV control.
	AMControl *bladesState_;
	/// The state control.
	BioXASSSRLMonochromatorMaskState *state_;
};

#endif // BIOXASSSRLMONOCHROMATORMASK_H
