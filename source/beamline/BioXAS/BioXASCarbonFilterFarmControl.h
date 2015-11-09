#ifndef BIOXASCARBONFILTERFARMCONTROL_H
#define BIOXASCARBONFILTERFARMCONTROL_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmFilterControl.h"

class BioXASCarbonFilterFarmControl : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarmControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmControl();

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
	/// Handles updating the filter control with the latest upstream filter information.
	void onUpstreamActuatorFilterChanged();
	/// Handles updating the filter control with the latest downstream filter information.
	void onDownstreamActuatorFilterChanged();

protected:
	/// The upstream actuator control.
	BioXASCarbonFilterFarmActuatorControl *upstreamActuator_;
	/// The downstream actuator control.
	BioXASCarbonFilterFarmActuatorControl *downstreamActuator_;
	/// The filter control.
	BioXASCarbonFilterFarmFilterControl *filter_;
};

#endif // BIOXASCARBONFILTERFARMCONTROL_H
