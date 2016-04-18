#ifndef BIOXASFASTSHUTTER_H
#define BIOXASFASTSHUTTER_H

#include "beamline/AMExclusiveStatesEnumeratedControl.h"

class BioXASFastShutter : public AMExclusiveStatesEnumeratedControl
{
    Q_OBJECT

public:
	/// Enumeration of the possible values.
	enum Value { Open = 0, Closed = 1 };

	/// Constructor.
	explicit BioXASFastShutter(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASFastShutter();

	/// Returns true if the fast shutter is open.
	virtual bool isOpen() const;
	/// Returns true if the fast shutter is closed.
	virtual bool isClosed() const;

signals:
	/// Notifier that the shutter operator control has changed.
	void operatorChanged(AMControl *newOperator);

public slots:
	/// Opens the fast shutter.
	AMControl::FailureExplanation open();
	/// Closes the fast shutter.
	AMControl::FailureExplanation close();

	/// Sets the operator control, the control used to move shutter states.
	void setOperatorControl(AMControl *newOperator, double openStatusValue, double openTrigger, double closedStatusValue, double closeTrigger);

protected:
	/// The operator control.
	AMControl *operator_;
};

#endif // BIOXASFASTSHUTTER_H
