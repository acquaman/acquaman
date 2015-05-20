#ifndef BIOXASM1MIRROR_H
#define BIOXASM1MIRROR_H

#include <QObject>

#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/CLS/CLSMAXvMotor.h"

class BioXASM1Mirror : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASM1Mirror(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASM1Mirror();

signals:

public slots:

protected:
	/// Updates the connected state.
	virtual void updateConnected();

protected:
	/// The vertical upstream inbound motor control.
	CLSMAXvMotor *verticalUpstreamInb_;
	/// The vertical upstream outbound motor control.
	CLSMAXvMotor *verticalUpstreamOutb_;
	/// The vertical downstream motor control.
	CLSMAXvMotor *verticalDownstream_;
	/// The stripe selection motor control.
	CLSMAXvMotor *stripeSelect_;
	/// The yaw motor control.
	CLSMAXvMotor *yaw_;
	/// The bender upstream motor control.
	CLSMAXvMotor *benderUpstream_;
	/// The bender downstream motor control.
	CLSMAXvMotor *benderDownstream_;

};

#endif // BIOXASM1MIRROR_H
