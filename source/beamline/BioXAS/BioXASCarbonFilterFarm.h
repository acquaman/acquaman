#ifndef BIOXASCARBONFILTERFARM_H
#define BIOXASCARBONFILTERFARM_H

#include <QObject>

#include "beamline/AMControl.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmControl.h"

class BioXASCarbonFilterFarm : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarm(const QString &name, const QString &upstreamPVBase, const QString &downstreamPVBase, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarm();

	/// Returns the name.
	QString name() const { return name_; }
	/// Returns true if the filter farm is connected, false otherwise.
	bool isConnected() const { return connected_; }

	/// Returns the total filter control.
	AMControl* filterControl() const { return filter_; }

	/// Returns the upstream actuator control.
	AMControl* upstreamActuatorControl() const { return upstreamActuator_; }
	/// Returns the upstream position control.
	AMControl* upstreamPositionControl() const { return upstreamPosition_; }
	/// Returns the upstream status control.
	AMControl* upstreamStatusControl() const { return upstreamStatus_; }

	/// Returns the downstream actuator control.
	AMControl* downstreamActuatorControl() const { return downstreamActuator_; }
	/// Returns the downstream position control.
	AMControl* downstreamPositionControl() const { return downstreamPosition_; }
	/// Returns the downstream status control.
	AMControl* downstreamStatusControl() const { return downstreamStatus_; }

signals:
	/// Notifier that the connected state has changed.
	void connectedChanged(bool isConnected);

protected slots:
	/// Sets the connected state.
	void setConnected(bool isConnected);

	/// Updates the connected state.
	void updateConnected();

protected:
	/// The name.
	QString name_;
	/// The current connected state.
	bool connected_;

	/// The total filter control.
	BioXASCarbonFilterFarmControl *filter_;

	/// The upstream actuator control.
	BioXASCarbonFilterFarmActuatorControl *upstreamActuator_;
	/// The upstream position motor control.
	CLSMAXvMotor *upstreamPosition_;
	/// The upstream status control.
	AMReadOnlyPVControl *upstreamStatus_;

	/// The downstream actuator control.
	BioXASCarbonFilterFarmActuatorControl *downstreamActuator_;
	/// The downstream position motor control.
	CLSMAXvMotor *downstreamPosition_;
	/// The downstream status control.
	AMReadOnlyPVControl *downstreamStatus_;
};

#endif // BIOXASCARBONFILTERFARM_H
