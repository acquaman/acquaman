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

<<<<<<< HEAD
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
=======
	/// Returns the upper slit blade motor control.
	CLSMAXvMotor* upperSlitBladeMotor() const { return upperSlitBladeMotor_; }

signals:
	/// Notifier that the upper slit blade motor control has changed.
	void upperSlitBladeMotorChanged(CLSMAXvMotor *newControl);

protected slots:
	/// Sets the upper slit blade motor control.
	void setUpperSlitBladeMotor(CLSMAXvMotor *newControl);

private:
	/// The upper slit blade motor control.
	CLSMAXvMotor *upperSlitBladeMotor_;
>>>>>>> masterBioXAS
};

#endif // BIOXASM1MIRROR_H
