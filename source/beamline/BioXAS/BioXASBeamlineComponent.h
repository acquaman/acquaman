#ifndef BIOXASBEAMLINECOMPONENT_H
#define BIOXASBEAMLINECOMPONENT_H

#include "beamline/AMConnectedControl.h"

class BioXASBeamlineComponent : public AMConnectedControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASBeamlineComponent(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamlineComponent();

	/// Returns true if this control should be able to be stopped (if connected), false otherwise.
	virtual bool shouldStop() const { return true; }
	/// Returns true if this control can be stopped right now, false otherwise. Finds this out be examining all child controls. Subclasses can reimplement to achieve their particular behavior.
	virtual bool canStop() const;

public slots:
	/// Stops all child controls that can be stopped.
	virtual bool stop();
};

#endif // BIOXASBEAMLINECOMPONENT_H
