#ifndef VESPERSMOTORGROUPVIEW_H
#define VESPERSMOTORGROUPVIEW_H

#include <QWidget>

#include <QDoubleSpinBox>
#include <QLabel>
#include <QToolButton>

#include "beamline/VESPERS/VESPERSMotorGroup.h"

/// This widget handles all of the views for the different motor group infos.  Depending on which info, will dictate how the view will look overall.
class VESPERSMotorGroupView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.  Requires a valid motor group.
	explicit VESPERSMotorGroupView(VESPERSMotorGroup *motorGroup, QWidget *parent = 0);

signals:

public slots:
	/// Sets a new info to be viewed.
	void setMotorGroupInfo(VESPERSMotorGroupInfo *info);

protected slots:
	/// Handles the popup menu that allows you to change the motors yo want to change.
	void onCustomContextMenuRequested(const QPoint &pos);

	/// Slot that handles going up.
	void onUpClicked();
	/// Slot that handles going down.
	void onDownClicked();
	/// Slot that handles going left.
	void onLeftClicked();
	/// Slot that handles going right.
	void onRightClicked();
	/// Slot that handles stopping.
	void onStopClicked();
	/// Slot that handles the first motor setpoint.
	void onFirstControlSetpoint();
	/// Slot that handles the second motor setpoint.
	void onSecondControlSetpoint();

	/// Handles changes in the moving flags of the sample stages.
	void onMovingChanged();
	/// Handles if the motors time out.
	void onTimedOut() { setEnabled(false); }

protected:
	/// Holds the motor group pointer.
	VESPERSMotorGroup *motorGroup_;
	/// Holds the current motor group info pointer.
	VESPERSMotorGroupInfo *currentMotorGroupInfo_;
	/// Holds the title label.
	QLabel *titleLabel_;
	/// Holds the first prefix label.
	QLabel *firstPrefixLabel_;
	/// Holds the second prefix label.
	QLabel *secondPrefixLabel_;
	/// Holds the first control spin box.
	QDoubleSpinBox *firstControlSetpoint_;
	/// Holds the second control spin box.
	QDoubleSpinBox *secondControlSetpoint_;
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
};

#endif // VESPERSMOTORGROUPVIEW_H
