#ifndef BIOXASMIRRORCONTROL_H
#define BIOXASMIRRORCONTROL_H

#include "beamline/AMPseudoMotorControl.h"

class BioXASMirrorControl : public AMPseudoMotorControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMirrorControl(const QString &name, const QString &units, double upstreamLength, double downstreamLength, QObject *parent = 0, const QString &description = QString());
	/// Destructor.
	virtual ~BioXASMirrorControl();

	/// Returns the upstream mirror length.
	double upstreamLength() const { return upstreamLength_; }
	/// Returns the downstream mirror length.
	double downstreamLength() const { return downstreamLength_; }

	/// Returns true if the current mirror lengths are valid, false otherwise.
	bool validLengths() const;

signals:
	/// Notifier that the upstream mirror length has changed.
	void upstreamLengthChanged(double newValue);
	/// Notifier that the downstream mirror length has changed.
	void downstreamLengthChanged(double newValue);

public slots:
	/// Sets the upstream mirror length.
	void setUpstreamLength(double newValue);
	/// Sets the downstream mirror length.
	void setDownstreamLength(double newValue);

protected:
	/// Creates and returns a move action.
	virtual AMAction3* createMoveAction(double setpoint);

	/// Creates and returns a move action iteration.
	virtual AMAction3* createMoveActionIteration(double setpoint) = 0;

protected:
	/// The upstream mirror length.
	double upstreamLength_;
	/// The downstream mirror length.
	double downstreamLength_;
};

#endif // BIOXASMIRRORCONTROL_H
