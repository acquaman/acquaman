#ifndef BIOXASMAINMONOCHROMATORREGIONCONTROL_H
#define BIOXASMAINMONOCHROMATORREGIONCONTROL_H

#include "beamline/AMControl.h"
#include "beamline/BioXAS/BioXASMainMonochromator.h"

class BioXASMainMonochromatorRegionControl : public AMControl
{
	Q_OBJECT

public:
	/// Enumerates the possible region states.
	class Region { public: enum State { None = 0, A, B }; };

	/// Constructor.
	explicit BioXASMainMonochromatorRegionControl(const QString &name, BioXASMainMonochromator *mono, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMainMonochromatorRegionControl();

	/// Returns the current region.
	virtual double value() const { return region_; }
	/// Returns the current region setpoint.
	virtual double setpoint() const { return regionSetpoint_; }
	/// Returns true if the control is connected.
	virtual bool isConnected() const;
	/// Returns true, the region should always be measurable if the control is connected.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true, the region should always be measurable if the control is connected.
	virtual bool canMeasure() const { return true; }
	/// Returns None, the smallest value the region can take.
	virtual double minimumValue() const { return Region::None; }
	/// Returns B, the largest value the region can take.
	virtual double maximumValue() const { return Region::B; }

	/// Returns the monochromator being controlled.
	BioXASMainMonochromator* mono() const { return mono_; }

signals:
	/// Notifier that the monochromator being controlled has changed.
	void monoChanged(BioXASMainMonochromator *newMono);
	/// Notifier that the region state has changed.
	void regionChanged(Region::State newState);

public slots:
	/// Sets the monochromator being controlled.
	void setMono(BioXASMainMonochromator *newMono);

protected slots:
	/// Sets the current region.
	void setRegion(Region::State newState);

protected:
	/// The monochromator being controlled.
	BioXASMainMonochromator *mono_;
	/// The current region state.
	Region::State region_;
	/// The current region setpoint.
	Region::State regionSetpoint_;

};

#endif // BIOXASMAINMONOCHROMATORREGIONCONTROL_H
