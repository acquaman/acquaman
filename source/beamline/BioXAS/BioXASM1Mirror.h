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

	/// Returns true if this control can stop, false otherwise.
	virtual bool canStop() const;

	/// Returns the current connected state.
	virtual bool isConnected() const;

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
};

#endif // BIOXASM1MIRROR_H
