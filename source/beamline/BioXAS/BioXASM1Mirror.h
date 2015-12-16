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

	/// Returns the current connected state. True if this control is connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the mask control.
	BioXASM1MirrorMask* mask() const { return mask_; }

signals:
	/// Notifier that the mask control has changed.
	void maskChanged(BioXASM1MirrorMask *newControl);

public slots:
	/// Sets the mask control.
	void setMask(BioXASM1MirrorMask *newControl);

protected:
	/// The mask control.
	BioXASM1MirrorMask *mask_;
};

#endif // BIOXASM1MIRROR_H
