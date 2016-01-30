#ifndef BIOXASFASTSHUTTER_H
#define BIOXASFASTSHUTTER_H

#include "beamline/AMExclusiveStatesEnumeratedControl.h"

class BioXASFastShutter : public AMExclusiveStatesEnumeratedControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASFastShutter(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASFastShutter();

signals:
	/// Notifier that the shutter operator control has changed.
	void operatorChanged(AMControl *newOperator);

public slots:
	/// Sets the operator control, the control used to move shutter states.
	void setOperatorControl(AMControl *newOperator, double openStatusValue, double openTrigger, double closedStatusValue, double closeTrigger);

protected:
	/// The operator control.
	AMControl *operator_;
};

#endif // BIOXASFASTSHUTTER_H
