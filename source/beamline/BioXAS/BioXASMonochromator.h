#ifndef BIOXASMONOCHROMATOR_H
#define BIOXASMONOCHROMATOR_H

#include <QObject>

#include "beamline/AMControl.h"
#include "actions3/AMAction3.h"

class BioXASMonochromator : public QObject
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASMonochromator(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMonochromator();

	/// Returns true if the mono is connected, false otherwise.
	virtual bool isConnected() const { return false; }

	/// Returns the energy control.
	virtual AMControl* energyControl() const = 0;

signals:
	/// Notifier that the mono's connected state has changed.
	void connectedChanged(bool isConnected);
};

#endif // BIOXASMONOCHROMATOR_H
