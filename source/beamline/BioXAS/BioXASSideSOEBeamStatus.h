#ifndef BIOXASSIDESOEBEAMSTATUS_H
#define BIOXASSIDESOEBEAMSTATUS_H

#include "beamline/BioXAS/BioXASSidePOEBeamStatus.h"

class CLSBiStateControl;

class BioXASSideSOEBeamStatus : public BioXASSidePOEBeamStatus
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideSOEBeamStatus(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideSOEBeamStatus();

	/// Returns true if this control is connected, false otherwise. Reimplemented to additionally consider the SOE-specific controls.
	virtual bool isConnected() const;

	/// Returns the endstation shutter control.
	CLSBiStateControl* endstationShutter() const { return endstationShutter_; }

signals:
	/// Notifier that the endstation shutter control has changed.
	void endstationShutterChanged(CLSBiStateControl *newControl);

public slots:
	/// Sets the endstation shutter control.
	virtual void setEndstationShutter(CLSBiStateControl *newControl);

protected:
	/// The endstation shutter control.
	CLSBiStateControl *endstationShutter_;
};

#endif // BIOXASSIDESOEBEAMSTATUS_H
