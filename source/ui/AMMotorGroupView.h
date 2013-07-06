#ifndef AMMOTORGROUPVIEW_H
#define AMMOTORGROUPVIEW_H

#include <QWidget>

#include <QDoubleSpinBox>
#include <QLabel>
#include <QToolButton>

#include "beamline/AMMotorGroup.h"

/// This widget handles building a view for an AMMotorGroupObject.  Builds the view based on all the information provided in the object.
class AMMotorGroupObjectView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Builds a view based around the provided \param motorGroupObject.
	explicit AMMotorGroupObjectView(AMMotorGroupObject *motorGroupObject, QWidget *parent = 0);

protected slots:
	/// Slot that handles going up.
	void onUpClicked();
	/// Slot that handles going down.
	void onDownClicked();
	/// Slot that handles going left.
	void onLeftClicked();
	/// Slot that handles going right.
	void onRightClicked();
	/// Slot that handles going in.
	void onInClicked();
	/// Slot that handles going out.
	void onOutClicked();
	/// Slot that handles stopping.
	void onStopClicked();
	/// Slot that handles the first motor setpoint.
	void onFirstControlSetpoint();
	/// Slot that handles the second motor setpoint.
	void onSecondControlSetpoint();
	/// Slot that handles the third motor setpoint.
	void onThirdControlSetpoint();

	/// Handles changes in the moving flags of the sample stages.
	void onMovingChanged();
	/// Handles if the motors time out.
	void onTimedOut() { setEnabled(false); }

protected:
	/// Holds the current motor group info pointer.
	AMMotorGroupObject *motorGroupObject_;

	/// Holds the title label.
	QLabel *titleLabel_;
	/// List that holds the prefix labels.
	QList<QLabel *> prefixLabels_;
	/// List that holds the control setpoint spin boxes.
	QList<QDoubleSpinBox *> controlSetpoints_;
	/// Holds the jog spin box.
	QDoubleSpinBox *jog_;
	/// Holds the status label;
	QLabel *status_;

	// Buttons.
	/// Stop button.
	QToolButton *stopButton_;
	/// The jog up button.
	QToolButton *goUp_;
	/// The jog down button.
	QToolButton *goDown_;
	/// The jog left button.
	QToolButton *goLeft_;
	/// The jog right button.
	QToolButton *goRight_;
	/// The jog in button.
	QToolButton *goIn_;
	/// The jog out button.
	QToolButton *goOut_;
};

/// This widget handles all of the AMMotorGroupObjectViews.
class AMMotorGroupView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Handles and builds all the views necessary for \param motorGroup.
	explicit AMMotorGroupView(AMMotorGroup *motorGroup, QWidget *parent = 0);

	/// Sets the view associated with a given name.
	void setMotorGroupView(const QString &name);

signals:

public slots:

protected slots:
	/// Handles the popup menu that allows you to change the motors yo want to change.
	void onCustomContextMenuRequested(const QPoint &pos);

protected:
	/// Holds the motor group pointer.
	AMMotorGroup *motorGroup_;
	/// Holds the map of views of motor group objects.
	QMap<QString, AMMotorGroupObjectView *> motorGroupViews_;
};

#endif // AMMOTORGROUPVIEW_H
