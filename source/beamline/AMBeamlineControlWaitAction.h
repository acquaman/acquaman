#ifndef AMBEAMLINECONTROLWAITACTION_H
#define AMBEAMLINECONTROLWAITACTION_H

#include "AMBeamlineActionItem.h"
#include "AMControl.h"

#include <QSlider>
#include <QToolButton>
#include <QButtonGroup>
#include <QApplication>
#include <QFile>

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

	virtual AMBeamlineActionItemView* createView(int index = 0);

	/// The AMControl that this action will operate on
	virtual AMControl* control();
	/// The value this action will wait for the control to have
	virtual double waitpoint() const;
	virtual double outlier() const;
	/// The tolerance this action will use for EqualTo and NotEqualTo (will be ignored for LessThan and GreaterThan)
	virtual double actionTolerance() const;
	/// The comparison this action will make (defined in waitTargetType enum)
	virtual int targetType() const;
	/// The amount of time (in milliseconds) this action will wait to make sure the condition is met. Attempting to avoid bouncing and noise near the threshold.
	virtual double holdTime() const;

signals:
	void progress(double elapsed, double total);
	void outlierValueChanged(double newOutlier);
	void controlValueChanged(double newValue);

public slots:
	virtual void start();
	virtual void cancel();
	virtual void setControl(AMControl *control);
	/// Sets the waitpoint for the action (must be set within the valid range of the control)
	virtual bool setWaitpoint(double waitpoint);
	/// Changes the tolerance for EqualTo and NotEqualTo (defaults to the tolerance of the inital control)
	virtual void setActionTolerance(double actionTolerance);
	/// Changes the comparison type for this action (from the waitTargetType enum)
	virtual void setTargetType(AMBeamlineControlWaitAction::waitTargetType targetType);
	/// Changes the amount of time (in milliseconds) this action will wait to make sure the condition is met. Attempting to avoid bouncing and noise near the threshold.
	virtual bool setHoldTime(double holdTime);

	virtual void cleanup();

protected slots:
	void delayedStart(bool ready);
	virtual void onValueChanged(double newValue);
	virtual void onConnected(bool connected);
	virtual void checkReady();
	virtual void calculateProgress();
	virtual void onHoldTimeReached();

protected:
	AMControl *control_;
	/// Value the control needs to reach
	double waitpoint_;
	/// Tolerance for EqualTo and NotEqualTo
	double actionTolerance_;
	/// Current type of comparison that will be done
	AMBeamlineControlWaitAction::waitTargetType targetType_;
	/// Value of the control when start is called (value could move towards OR away from the waitpoint afterwards)
	double startpoint_;
	/// Furthest value away from the desired waitpoint (calculated and updated during calculateProgress())
	double outlierpoint_;
	/// Timer that triggers a new calculation of the progress
	QTimer progressTimer_;
	/// How long the action will wait to make sure the desired value does not retreat past the threshold
	double holdTime_; //In milliseconds
	/// Timer for the holdTime
	QTimer holdTimeTimer_; //How long the wait needs to be held
};

class AMDoubleSlider;

class AMBeamlineControlWaitDetailedActionView : public AMBeamlineActionItemView
{
	Q_OBJECT
public:
	AMBeamlineControlWaitDetailedActionView(AMBeamlineControlWaitAction *waitAction, int index = 0, QWidget *parent = 0);

public slots:
	void setIndex(int index);
	virtual void setAction(AMBeamlineActionItem *action);

signals:
	void actionStarted(AMBeamlineActionItem *action);
	void actionSucceeded(AMBeamlineActionItem *action);
	void actionFailed(AMBeamlineActionItem *action);

protected slots:
	virtual void onInfoChanged();
	virtual void onStopCancelButtonClicked();
	virtual void onPlayPauseButtonClicked();

	void onActionStarted();
	void onActionSucceeded();
	void onActionFailed(int explanation);

	void onHelpButtonClicked();

protected:
	AMBeamlineControlWaitAction *waitAction_;

	QLabel *messageLabel_;
	AMDoubleSlider *progressSlider_;
	QToolButton *finishedState_;
	QToolButton *helpButton_;
	QHBoxLayout *mainHL_;
};

class AMDoubleSlider : public QWidget
{
Q_OBJECT
public:
	enum scalingModeType {
		linear = 1,
		logrithmic
	};

	enum destinationType {
		destinationRight = 1,
		destinationLeft
	};

	AMDoubleSlider(AMDoubleSlider::scalingModeType scalingMode = AMDoubleSlider::linear, AMDoubleSlider::destinationType destinationMode = AMDoubleSlider::destinationLeft, QWidget *parent = 0);

	double currentValue() const;
	double minimum() const;
	double maximum() const;
	AMDoubleSlider::scalingModeType scalingMode() const;

public slots:
	void setCurrentValue(double value);
	void setMinimum(double minimum);
	void setMaximum(double maximum);
	void setScalingMode(int scalingMode);

protected:
	bool trySetScalingMode(int scalingMode);

protected slots:
	void onScalingButtonClicked(int scalingMode);

protected:
	double curValue_;
	double minValue_;
	double maxValue_;
	AMDoubleSlider::scalingModeType scalingMode_;
	AMDoubleSlider::destinationType destinationMode_;

	QSlider *slider_;
	QLabel *valLabel_;
	QLabel *minLabel_;
	QLabel *maxLabel_;
	QButtonGroup *scalingGroup_;
	QToolButton *linearButton_;
	QToolButton *logrithmicButton_;
	QVBoxLayout *vl_;
};

#endif // AMBEAMLINECONTROLWAITACTION_H
