#ifndef AMBEAMLINECONTROLWAITACTION_H
#define AMBEAMLINECONTROLWAITACTION_H

#include "AMBeamlineActionItem.h"
#include "AMControl.h"

// Wait until a control reaches a certain value. We're not moving it, it's changing on its own.

class AMBeamlineControlWaitAction : public AMBeamlineActionItem
{
Q_OBJECT
public:
	enum waitTargetType {
		LessThanTarget = 1, ///< Waits until the feedback value is less than the waitpoint
		GreaterThanTarget, ///< Waits until the feedback value is greater than the waitpoint
		EqualToTarget, ///< Waits until the feedback value is equal to the waitpoint within a defined tolerance
		NotEqualToTarget ///< Waits until the feedback value is not equal to the waitpoint beyond a defined tolerance
	};

	explicit AMBeamlineControlWaitAction(AMControl *control, AMBeamlineControlWaitAction::waitTargetType targetType, QObject *parent = 0);

	virtual AMBeamlineActionView* createView(int index = 0);

	virtual AMControl* control();
	virtual double waitpoint() const;
	virtual double actionTolerance() const;
	virtual int targetType() const;
	virtual double holdTime() const;

signals:
	void progress(double elapsed, double total);

public slots:
	virtual void start();
	virtual void cancel();
	virtual void setControl(AMControl *control);
	virtual bool setWaitpoint(double waitpoint);
	virtual void setActionTolerance(double actionTolerance);
	virtual void setTargetType(AMBeamlineControlWaitAction::waitTargetType targetType);
	virtual bool setHoldTime(double holdTime);
	virtual void cleanup();

protected slots:
	void delayedStart(bool ready);
	virtual void onValueChanged(double newValue);
	virtual void onConnected(bool connected);
	virtual void checkReady();
	virtual void onStarted();
	virtual void calculateProgress();
	virtual void onHoldTimeReached();

protected:
	AMControl *control_;
	double waitpoint_;
	double actionTolerance_;
	AMBeamlineControlWaitAction::waitTargetType targetType_;
	double startpoint_;
	QTimer progressTimer_;
	double holdTime_; //In milliseconds
	QTimer holdTimeTimer_; //How long the wait needs to be held
};

#endif // AMBEAMLINECONTROLWAITACTION_H
