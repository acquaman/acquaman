#ifndef SGMLADDERCONTROL_H
#define SGMLADDERCONTROL_H

#include "beamline/AMSingleEnumeratedControl.h"
#include "beamline/AMPVControl.h"

class SGMLadderControl : public AMSingleEnumeratedControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit SGMLadderControl(const QString &name, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~SGMLadderControl();

	/// Returns the motor control.
	AMPVControl* motor() const { return qobject_cast<AMPVControl*>(control_); }

protected slots:
	/// Adds a ladder option.
	void addLadderOption(const QString &optionName, double setpoint, double min, double max);
	/// Removes a ladder option.
	void removeLadderOption(const QString &optionName);
	/// Clears all ladder options.
	void clearLadderOptions();
};

#endif // SGMLADDERCONTROL_H
