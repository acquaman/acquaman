#ifndef BIOXASXIAFILTERS_H
#define BIOXASXIAFILTERS_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/BioXAS/BioXASXIAFiltersActuatorControl.h"

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

	/// Returns the first filter control.
	BioXASXIAFiltersActuatorControl* filter1() const { return filter1_; }
	/// Returns the second filter control.
	BioXASXIAFiltersActuatorControl* filter2() const { return filter2_; }
	/// Returns the third filter control.
	BioXASXIAFiltersActuatorControl* filter3() const { return filter3_; }
	/// Returns the fourth filter control.
	BioXASXIAFiltersActuatorControl* filter4() const { return filter4_; }

protected:
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
