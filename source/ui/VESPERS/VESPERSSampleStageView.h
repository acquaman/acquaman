#ifndef VESPERSSAMPLESTAGEVIEW_H
#define VESPERSSAMPLESTAGEVIEW_H

#include "beamline/VESPERS/VESPERSBeamline.h"

#include <QWidget>
#include <QDoubleSpinBox>
#include <QButtonGroup>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QToolButton>

/*! This class builds a view on the sample stage control that does relative movement for the sample stage.  It does not change the focus distance,
  only the horizontal and vertical motors.
  */
class VESPERSSampleStageView : public QWidget
{
	Q_OBJECT
public:
	/// Default constructor.
	explicit VESPERSSampleStageView(QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Slot that handles going up.
	void onUpClicked();
	/// Slot that handles going down.
	void onDownClicked();
	/// Slot that handles going left.
	void onLeftClicked();
	/// Slot that handles going right.
	void onRightClicked();
	/// Slot that handles if the horizontal setpoint line edit is triggered.
	void onHorizontalSetpoint();
	/// Slot that handles if the vertical setpoint line edit is triggered.
	void onVerticalSetpoint();

	/// Handles changes in the moving flags of the sample stages.
	void onMovingChanged(bool isMoving);
	/// Handles if the motors time out.
	void onTimedOut() { setEnabled(false); }
	/// Handles changes from the horizontal motor from the sample stage.
	void onHorizontalChanged(double val);
	/// Handles changes from the vertical motor from the sample stage.
	void onVerticalChanged(double val);
	/// Handles the error message from a horizontal move error from the sample stage.
	void onHorizontalMoveError(bool direction);
	/// Handles the error message from a vertical move error from the sample stage.
	void onVerticalMoveError(bool direction);
	/// Handles the error message from a normal move error from the sample stage.
	void onNormalMoveError(bool direction);

protected:
	/// Holds the jog value.  All movements are relative to this number.
	QDoubleSpinBox *jog_;
	/// This button group holds the four directional buttons.
	QButtonGroup *buttons_;
	/// The status label that shows whether the sample stage is moving or not.
	QLabel *status_;
	/// Holds the horizontal setpoint.
	QLineEdit *horizontal_;
	/// Holds the vertical setpoint.
	QLineEdit *vertical_;

	// Jog buttons.
	/// The jog up button.
	QToolButton *goUp_;
	/// The jog down button.
	QToolButton *goDown_;
	/// The jog left button.
	QToolButton *goLeft_;
	/// The jog right button.
	QToolButton *goRight_;

	/// The control for the sample stage.
	SampleStageControl *sampleStage_;
};

#endif // VESPERSSAMPLESTAGEVIEW_H
