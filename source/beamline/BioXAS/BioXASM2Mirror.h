#ifndef BIOXASM2MIRROR_H
#define BIOXASM2MIRROR_H

#include <QObject>

#include "beamline/AMPVControl.h"

class CLSMAXvMotor;
class BioXASPseudoMotorControl;

class BioXASM2Mirror : public QObject
{
    Q_OBJECT

public:
	/// Enum class for the fluorescence screen positions.
	class Screen { public: enum Position { In = 0, Out, Invalid }; };

	/// Constructor.
	explicit BioXASM2Mirror(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASM2Mirror();

	/// Returns true if the mirror is connected, false otherwise.
	bool isConnected() const { return isConnected_; }

	/// Returns the fluorescence screen control.
	AMPVControl* screenControl() const { return screen_; }
	/// Returns the vertical upstream inbound motor control.
	CLSMAXvMotor* verticalUpstreamInboundControl() const { return verticalUpstreamInb_; }
	/// Returns the vertical upstream outbound motor control.
	CLSMAXvMotor* verticalUpstreamOutboundControl() const { return verticalUpstreamOutb_; }
	/// Returns the vertical downstream motor control.
	CLSMAXvMotor* verticalDownstreamControl() const { return verticalDownstream_; }
	/// Returns the stripe selection control.
	CLSMAXvMotor* stripeSelectControl() const { return stripeSelect_; }
	/// Returns the yaw control.
	CLSMAXvMotor* yawControl() const { return yaw_; }
	/// Returns the bender upstream control.
	CLSMAXvMotor* benderUpstreamControl() const { return benderUpstream_; }
	/// Returns the bender downstream control.
	CLSMAXvMotor* benderDownstreamControl() const { return benderDownstream_; }

	/// Returns the roll pseudo motor control.
	BioXASPseudoMotorControl* pseudoRollControl() const { return pseudoRoll_; }
	/// The pitch pseudo motor control.
	BioXASPseudoMotorControl* pseudoPitchControl() const { return pseudoPitch_; }
	/// The yaw pseudo motor control.
	BioXASPseudoMotorControl* pseudoYawControl() const { return pseudoYaw_; }
	/// The height pseudo motor control.
	BioXASPseudoMotorControl* pseudoHeightControl() const { return pseudoHeight_; }
	/// The lateral pseudo motor control.
	BioXASPseudoMotorControl* pseudoLateralControl() const { return pseudoLateral_; }

signals:
	/// Notifier that the mirror's connected state has changed.
	void connected(bool isConnected);

public slots:
	/// Sets the screen control position.
	void setScreenPosition(Screen::Position newPosition);

protected slots:
	/// Sets the current connected state.
	void setConnected(bool isConnected);

	/// Handles updating the current connected state.
	void onConnectedChanged();

protected:
	/// The current connected state.
	bool isConnected_;

	/// The mirror fluorescence screen control.
	AMSinglePVControl *screen_;

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

	/// The roll pseudo motor control.
	BioXASPseudoMotorControl *pseudoRoll_;
	/// The pitch pseudo motor control.
	BioXASPseudoMotorControl *pseudoPitch_;
	/// The yaw pseudo motor control.
	BioXASPseudoMotorControl *pseudoYaw_;
	/// The height pseudo motor control.
	BioXASPseudoMotorControl *pseudoHeight_;
	/// The lateral pseudo motor control.
	BioXASPseudoMotorControl *pseudoLateral_;

};

#endif // BIOXASM2MIRROR_H
