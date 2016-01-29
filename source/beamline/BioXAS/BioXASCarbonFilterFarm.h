#ifndef BIOXASCARBONFILTERFARM_H
#define BIOXASCARBONFILTERFARM_H

#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmFilterControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuator.h"


class BioXASCarbonFilterFarm : public BioXASBeamlineComponent
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarm(const QString &deviceName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarm();

	/// Returns the current connected state.
	virtual bool isConnected() const;

	/// Returns the upstream actuator control.
	BioXASCarbonFilterFarmActuatorControl* upstreamActuator() const { return upstreamActuator_; }
	/// Returns the downstream actuator control.
	BioXASCarbonFilterFarmActuatorControl* downstreamActuator() const { return downstreamActuator_; }
	/// Returns the filter control.
	BioXASCarbonFilterFarmFilterControl* filter() const { return filter_; }

signals:
	/// Notifier that the upstream actuator control has changed.
	void upstreamActuatorChanged(AMControl *newControl);
	/// Notifier that the downstream actuator control has changed.
	void downstreamActuatorChanged(AMControl *newControl);
	/// Notifier that the filter control has changed.
	void filterChanged(AMControl *filter);

public slots:
	/// Sets the upstream actuator control.
	void setUpstreamActuator(BioXASCarbonFilterFarmActuatorControl *newControl);
	/// Sets the downstream actuator control.
	void setDownstreamActuator(BioXASCarbonFilterFarmActuatorControl *newControl);
	/// Sets the filter control.
	void setFilter(BioXASCarbonFilterFarmFilterControl *newControl);

protected slots:
	/// Updates the upstream filter control.
	void updateUpstreamFilter();
	/// Updates the downstream filter control.
	void updateDownstreamFilter();
	/// Updates the filter control with the appropriate upstream and downstream filters.
	void updateFilter();

protected:
	/// The upstream actuator control.
	BioXASCarbonFilterFarmActuatorControl *upstreamActuator_;
	/// The downstream actuator control.
	BioXASCarbonFilterFarmActuatorControl *downstreamActuator_;
	/// The filter control.
	BioXASCarbonFilterFarmFilterControl *filter_;
};

#endif // BIOXASCARBONFILTERFARM_H
