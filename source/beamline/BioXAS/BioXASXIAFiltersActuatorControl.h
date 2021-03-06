#ifndef BIOXASXIAFILTERSACTUATORCONTROL_H
#define BIOXASXIAFILTERSACTUATORCONTROL_H

#include "beamline/AMExclusiveStatesEnumeratedControl.h"
#include "beamline/AMPVControl.h"

class BioXASXIAFiltersActuatorControl : public AMExclusiveStatesEnumeratedControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASXIAFiltersActuatorControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASXIAFiltersActuatorControl();

signals:
	/// Notifier that the state options have changed.
	void statesChanged();

public slots:
	/// Adds a filter state.
	void addFilterState(int index, const QString &stateName, AMControl *control);
};

#endif // BIOXASXIAFILTERSACTUATORCONTROL_H
