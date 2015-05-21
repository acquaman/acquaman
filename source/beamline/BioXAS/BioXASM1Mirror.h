#ifndef BIOXASM1MIRROR_H
#define BIOXASM1MIRROR_H

#include <QObject>

#include "beamline/BioXAS/BioXASMirror.h"

class BioXASM1Mirror : public BioXASMirror
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASM1Mirror(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASM1Mirror();

	/// Returns the upper slit blade control.
	AMControl* upperSlitBladeControl() const { return upperSlitBlade_; }

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();

protected:
	/// The upper slit blade motor control.
	CLSMAXvMotor *upperSlitBlade_;
};

#endif // BIOXASM1MIRROR_H
