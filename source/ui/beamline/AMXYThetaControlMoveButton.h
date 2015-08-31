#ifndef AMXYTHETACONTROLMOVEBUTTON_H
#define AMXYTHETACONTROLMOVEBUTTON_H

#include <QToolButton>
#include <QList>
#include <QDialog>
#include <QButtonGroup>

class AMControl;
class AMXYThetaControlMoveButtonContextMenu;

#include "AMControlMoveButton.h"

class AMXYThetaControlMoveButton : public QToolButton
{
	Q_OBJECT
public:

	/// Create a move button for the given set of controls that moves along both X and Y based on the current value from Theta to allow movemnt parallel and perpendicular to Theta (X', Y'). Assume X and Y use the same units and Theta feedback is in degrees.  The available move step sizes (in the controlX's units) are given by \c stepSizes.  The four combinations of y \c directionXReversed and \c directionYReversed, allow for movement along any of +/-X',+/-Y' regardless of what axis the angle is measure wrt.  It's okay for \c control to be 0. This button will simply do nothing (and be disabled) until you call setControl() with a valid control.  This widget does not take ownership of the control.
	virtual ~AMXYThetaControlMoveButton(){}
	explicit AMXYThetaControlMoveButton(QWidget *parent = 0,
								 AMControl* controlX = 0, AMControl* controlY = 0, AMControl* controlTheta = 0,
								 QList<double> stepSizes = QList<double>() << 1 << 5 << 10,
								 bool directionXReversed = false, bool directionYReversed = false, double angleOffest = 0);

	/// Returns the control that is moved by this button. Could be 0 if this button has no control set.
	AMControl* controlX() const { return controlX_; }
	/// Returns the first control that is moved by this button. Could be 0 if this button has no control set.
	AMControl* controlY() const { return controlY_; }
	/// Returns the control that is moved by this button. Could be 0 if this button has no control set.
	AMControl* controlTheta() const { return controlTheta_; }


	/// Set the first control that is moved by this button. It's okay for \c control to be 0, in which case this button will simply do nothing until you call setControl() again with a valid control.
	void setControlX(AMControl* controlX);
	/// Set the second control that is moved by this button. It's okay for \c control to be 0, in which case this button will simply do nothing until you call setControl() again with a valid control.
	void setControlY(AMControl* controlY);
	/// Set the rotation control that is moved by this button. It's okay for \c control to be 0, in which case this button will simply do nothing until you call setControl() again with a valid control.
	void setControlTheta(AMControl* controlTheta);

	/// Returns the distance of the move step that will be taken when the button is clicked.
	double currentStepSize() const { return stepSizes_.at(stepSizeIndex_); }
	/// Returns the available step sizes that users can choose from with the right-click panel.
	QList<double> stepSizes() const { return stepSizes_; }
	/// Returns the index in stepSizes() of the current step size.
	int stepSizeIndex() const {return stepSizeIndex_; }

	/// Returns true if the move direction should be opposite the signs given in stepSizes().
	bool directionXReversed() const { return directionXReversed_; }
	/// Returns true if the move direction should be opposite the signs given in stepSizes().
	bool directionYReversed() const { return directionYReversed_; }

	/// Returns the current Angle Offset value
	double angleOffset() const {return angleOffset_; }

public slots:
	/// Select one of the available step sizes, from the list of stepSizes().
	bool setStepSizeIndex(int index);
	/// Sets the available step sizes that can be chosen (and are shown in the right-click panel). Returns false and does nothing if empty. The current stepSizeIndex() is reset to 0.
	bool setStepSizes(const QList<double>& stepSizes);

	/// Specifies to reverse the direction of the x-axis moves.
	void setDirectionXReversed(bool trueForReverse) { directionXReversed_ = trueForReverse; }
	/// Specifies to reverse the direction of the y-axis moves.
	void setDirectionYReversed(bool trueForReverse) { directionYReversed_ = trueForReverse; }
	/// Specifies the angle offest to adjust plane of motion
	void setAngleOffset(double  newAngleOffset) { angleOffset_ = newAngleOffset; }

	/// "Press" the button: When set to true, the button's down state is activated.  When changed to false, the button is changed to the up state, and a click() is generated.  \note If the button is already in the up state when press(false) is called, no click() will be generated.
	void press(bool down = true);
	/// Synonym for press(false)
	void release() { press(false); }

signals:
	/// Emitted when the step size changes, with the index in stepSizes(). Used by the right-click panel AMControlMoveButtonContextMenu.
	void stepSizeIndexChanged(int newIndex);

protected slots:
	/// When the button is clicked, relativeMove()s will be initiated on control() and control2(), based on distance of currentStepSize() and controlR().value().
	virtual void onButtonClicked();
	/// When the existing controlX is destroyed, we must set our control to 0.
	void onControlXDestroyed();
	/// When the existing controlY is destroyed, we must set our control to 0.
	void onControlYDestroyed();
	/// When the existing controlTheta is destroyed, we must set our control to 0.
	void onControlThetaDestroyed();
	/// Checkes all controls and updates the button state
	void checkStatus();
	/// Called when the button is right-clicked to request a custom context menu
	void onCustomContextMenuRequested(const QPoint& point);



protected:
	AMControl* controlX_;
	AMControl* controlY_;
	AMControl* controlTheta_;
	AMXYThetaControlMoveButtonContextMenu* contextMenu_;
	bool directionXReversed_;
	bool directionYReversed_;
	QList<double> stepSizes_;
	int stepSizeIndex_;
	double angleOffset_;


};

/// This widget is used as a custom context menu for AMControlMoveButton. It lets the user choose from a range of step sizes, and provides an AMControlEditor for viewing and setting the absolute position.
class AMXYThetaControlMoveButtonContextMenu : public QDialog {
	Q_OBJECT
public:
	virtual ~AMXYThetaControlMoveButtonContextMenu();
	AMXYThetaControlMoveButtonContextMenu(AMXYThetaControlMoveButton* moveButton);

protected slots:
	void onStepSizeIndexChanged(int newIndex);

protected:
	void leaveEvent(QEvent *);

	AMXYThetaControlMoveButton* moveButton_;
	QButtonGroup stepSizeButtons_;
};



#endif // AMXYTHETACONTROLMOVEBUTTON_H



