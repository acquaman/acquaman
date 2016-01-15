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
	/// Notifier that the shutter status control has changed.
	void statusChanged(AMControl *newStatus);
	/// Notifier that the shutter operator control has changed.
	void operatorChanged(AMControl *newOperator);

public slots:
	/// Sets the shutter status control.
	void setStatus(AMControl *newStatus);
	/// Sets the operator control, the control used to move shutter states.
	void setOperator(AMControl *newOperator);

protected:
	/// The operator control.
	AMControl *operator_;
};

#endif // BIOXASFASTSHUTTER_H
