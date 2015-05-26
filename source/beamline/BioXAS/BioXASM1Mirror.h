#ifndef BIOXASM1MIRROR_H
#define BIOXASM1MIRROR_H

#include <QObject>

#include "beamline/BioXAS/BioXASMirror.h"

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

	/// Returns the upper slit blade control.
	AMControl* upperSlitBladeControl() const { return upperSlitBlade_; }

protected:
	/// The upper slit blade motor control.
	CLSMAXvMotor *upperSlitBlade_;
};

#endif // BIOXASM1MIRROR_H
