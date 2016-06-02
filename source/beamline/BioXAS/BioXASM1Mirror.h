#ifndef BIOXASM1MIRROR_H
#define BIOXASM1MIRROR_H

#include <QObject>

#include "beamline/BioXAS/BioXASMirror.h"

class BioXASM1MirrorMask;

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

	/// Returns the mask control.
	BioXASM1MirrorMask* mask() const { return mask_; }
	/// Returns the downstream blade current control.
	AMControl* downstreamBladeCurrent() const { return downstreamBladeCurrent_; }

signals:
	/// Notifier that the mask control has changed.
	void maskChanged(BioXASM1MirrorMask *newControl);
	/// Notifier that the downstream blade current control has changed.
	void downstreamBladeCurrentChanged(AMControl *newControl);

public slots:
	/// Sets the mask control.
	void setMask(BioXASM1MirrorMask *newControl);
	/// Sets the downstream blade current control.
	void setDownstreamBladeCurrent(AMControl *newControl);

protected:
	/// The mask control.
	BioXASM1MirrorMask *mask_;
	/// The downstream blade current.
	AMControl *downstreamBladeCurrent_;
};

#endif // BIOXASM1MIRROR_H
