#ifndef BIOXASXIAFILTERS_H
#define BIOXASXIAFILTERS_H

#include <QObject>
#include <QSignalMapper>

#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/BioXAS/BioXASXIAFiltersControl.h"
#include "beamline/BioXAS/BioXASXIAFiltersActuator.h"

// Setpoints.
#define BIOXAS_XIA_FILTERS_POSITION_OUT -50
#define BIOXAS_XIA_FILTERS_POSITION_IN 50
#define BIOXAS_XIA_FILTERS_POSITION_TOLERANCE 0.1

// Error codes.
#define BIOXAS_XIA_FILTERS_NOT_CONNECTED 29348721
#define BIOXAS_XIA_FILTERS_ALREADY_MOVING 29348722
#define BIOXAS_XIA_FILTERS_INVALID_SETPOINT 29348723
#define BIOXAS_XIA_FILTERS_MOVE_FAILED 29348724

class BioXASXIAFilters : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASXIAFilters(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASXIAFilters();

	/// Returns the current connected state.
	virtual bool isConnected() const;

	/// Returns the filters control.
	BioXASXIAFiltersControl *filtersControl() const { return filtersControl_; }

	/// Returns the first filter control.
	BioXASXIAFiltersActuatorControl* filter1() const { return filter1_; }
	/// Returns the second filter control.
	BioXASXIAFiltersActuatorControl* filter2() const { return filter2_; }
	/// Returns the third filter control.
	BioXASXIAFiltersActuatorControl* filter3() const { return filter3_; }
	/// Returns the fourth filter control.
	BioXASXIAFiltersActuatorControl* filter4() const { return filter4_; }

protected:
	/// The filters control.
	BioXASXIAFiltersControl *filtersControl_;

	/// Control for the first filter.
	BioXASXIAFiltersActuatorControl *filter1_;
	/// Control for the second filter.
	BioXASXIAFiltersActuatorControl *filter2_;
	/// Control for the third filter.
	BioXASXIAFiltersActuatorControl *filter3_;
	/// Control for the fourth filter.
	BioXASXIAFiltersActuatorControl *filter4_;
};

#endif // BIOXASXIAFILTERS_H
