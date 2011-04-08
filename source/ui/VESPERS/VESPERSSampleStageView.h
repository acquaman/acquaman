#ifndef VESPERSSAMPLESTAGEVIEW_H
#define VESPERSSAMPLESTAGEVIEW_H

#include "beamline/VESPERS/VESPERSBeamline.h"
#include "beamline/AMControl.h"

#include <QWidget>
#include <QDoubleSpinBox>
#include <QButtonGroup>

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
	/// Handles changes in the moving flags of the sample stages.
	void onMovingChanged(bool isMoving);
	/// Handles changes in the connection of the sample stages.
	void onConnectedChanged(bool isConnected);
	/// Handles if the motors time out.
	void onTimedOut() { onConnectedChanged(false); }

protected:
	/// Holds the jog value.  All movements are relative to this number.
	QDoubleSpinBox *jog_;
	/// This button group holds the four directional buttons.
	QButtonGroup *buttons_;

	/// The control for the horizontal sample stage control.
	AMPVwStatusControl *horizontal_;
	/// The control for the vertical sample stage control.
	AMPVwStatusControl *vertical_;
};

#endif // VESPERSSAMPLESTAGEVIEW_H
