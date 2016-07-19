#ifndef BIOXASM1MIRROR_H
#define BIOXASM1MIRROR_H

#include <QObject>

#include "beamline/BioXAS/BioXASMirror.h"
#include "beamline/BioXAS/BioXASMirrorMaskState.h"

class BioXASM1Mirror : public BioXASMirror
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASM1Mirror(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASM1Mirror();

	/// Returns true if this control can stop, false otherwise.
	virtual bool canStop() const;

	/// Returns the current connected state.
	virtual bool isConnected() const;

	/// Returns the mask upper blade control.
	AMControl* maskUpperBlade() const { return maskUpperBlade_; }
	/// Returns the mask state control.
	BioXASMirrorMaskState* maskState() const { return maskState_; }
	/// Returns the downstream blade current control.
	AMControl* downstreamBladeCurrent() const { return downstreamBladeCurrent_; }

signals:
	/// Notifier that the mask upper blade control has changed.
	void maskUpperBladeChanged(AMControl *newControl);
	/// Notifier that the mask state control has changed.
	void maskStateChanged(BioXASMirrorMaskState *newControl);
	/// Notifier that the downstream blade current control has changed.
	void downstreamBladeCurrentChanged(AMControl *newControl);

public slots:
	/// Sets the mask upper blade control.
	void setMaskUpperBlade(AMControl *newControl);
	/// Sets the mask state control.
	void setMaskState(BioXASMirrorMaskState *newControl);
	/// Sets the downstream blade current control.
	void setDownstreamBladeCurrent(AMControl *newControl);

protected slots:
	/// Updates the mask state control.
	void updateMaskState();

protected:
	/// The mask upper blade control.
	AMControl *maskUpperBlade_;
	/// The mask state control.
	BioXASMirrorMaskState *maskState_;
	/// The downstream blade current.
	AMControl *downstreamBladeCurrent_;
};

#endif // BIOXASM1MIRROR_H
