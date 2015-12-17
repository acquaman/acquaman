#ifndef BIOXASM1MIRRORMASK_H
#define BIOXASM1MIRRORMASK_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"

class BioXASM1MirrorMaskState;

class BioXASM1MirrorMask : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASM1MirrorMask(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASM1MirrorMask();

	/// Returns true if connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the upper slit blade control.
	AMControl* upperSlitBlade() const { return upperSlitBlade_; }
	/// Returns the state control.
	BioXASM1MirrorMaskState* state() const { return state_; }

signals:
	/// Notifier that the upper slit blade control has changed.
	void upperSlitBladeChanged(AMControl *newControl);
	/// Notifier that the state control has changed.
	void stateChanged(BioXASM1MirrorMaskState *newControl);

public slots:
	/// Sets the upper slit blade control.
	void setUpperSlitBlade(AMControl *newControl);
	/// Sets the state control.
	void setState(BioXASM1MirrorMaskState *newControl);

protected slots:
	/// Updates the state control.
	void updateState();
	/// Updates the mask control.
	void updateMask();

protected:
	/// The upper slit blade control.
	AMControl *upperSlitBlade_;
	/// The state control.
	BioXASM1MirrorMaskState *state_;
};

#endif // BIOXASM1MIRRORMASK_H
