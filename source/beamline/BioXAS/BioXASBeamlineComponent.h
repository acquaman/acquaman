#ifndef BIOXASBEAMLINECOMPONENT_H
#define BIOXASBEAMLINECOMPONENT_H

#include "beamline/AMControl.h"

class BioXASBeamlineComponent : public AMControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASBeamlineComponent(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamlineComponent();

protected slots:
	/// Sets the current connected state.
	void setConnected(bool isConnected);
	/// Updates the connected state.
	virtual void updateConnected();

protected:
	/// The current connected state.
	bool connected_;

};

#endif // BIOXASBEAMLINECOMPONENT_H
