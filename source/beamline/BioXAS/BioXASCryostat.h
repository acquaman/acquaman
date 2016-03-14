#ifndef BIOXASCRYOSTAT_H
#define BIOXASCRYOSTAT_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"

class BioXASCryostat : public BioXASBeamlineComponent
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASCryostat(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCryostat();

	/// Returns the pressure value.
	double pressureValue() const;
	/// Returns the temperature value.
	double temperatureValue() const;

	/// Returns the pressure control.
	AMControl* pressure() const { return pressure_; }
	/// Returns the temperature control.
	AMControl* temperature() const { return temperature_; }

signals:
	/// Notifier that the pressure value has changed.
	void pressureValueChanged(double newValue);
	/// Notifier that the temperature value has changed.
	void temperatureValueChanged(double newValue);
	/// Notifier that the pressure control has changed.
	void pressureChanged(AMControl *newControl);
	/// Notifier that the temperature control has changed.
	void temperatureChanged(AMControl *newControl);

public slots:
	/// Sets the pressure control.
	void setPressure(AMControl *newControl);
	/// Sets the temperature control.
	void setTemperature(AMControl *newControl);

protected:
	/// The pressure control.
	AMControl *pressure_;
	/// The temperature control.
	AMControl *temperature_;
};

#endif // BIOXASCRYOSTAT_H
