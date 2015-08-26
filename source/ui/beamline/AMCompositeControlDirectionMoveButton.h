#ifndef AMCOMPOSITECONTROLDIRECTIONMOVEBUTTON_H
#define AMCOMPOSITECONTROLDIRECTIONMOVEBUTTON_H

#include <QToolButton>
#include <QList>
#include <QDialog>
#include <QButtonGroup>

class AMControl;
class AMCompositeControlMoveButtonContextMenu;

#include "AMControlMoveButton.h"

class AMCompositeControlDirectionMoveButton : public AMControlMoveButton
{
	Q_OBJECT
public:

	/// Create a move button for the given \c control. The available move step sizes (in the control's units) are given by \c stepSizes.  If you specify \c reverseDirection, the direction of the move will be opposite of the sign of the values in stepSizes.  It's okay for \c control to be 0. This button will simply do nothing (and be disabled) until you call setControl() with a valid control.  This widget does not take ownership of the control.
	virtual ~AMCompositeControlDirectionMoveButton(){}
	explicit AMCompositeControlDirectionMoveButton(QWidget *parent = 0,
								 AMControl* control = 0, AMControl* control2 = 0, AMControl* controlR = 0,
								 QList<double> stepSizes = QList<double>() << 1 << 5 << 10,
								 bool directionReversed = false, bool direction2Reversed = false);

	/// Set the first control that is moved by this button. It's okay for \c control to be 0, in which case this button will simply do nothing until you call setControl() again with a valid control.
	void setControl(AMControl* control);
	/// Set the second control that is moved by this button. It's okay for \c control to be 0, in which case this button will simply do nothing until you call setControl() again with a valid control.
	void setControl2(AMControl* control2);
	/// Set the rotation control that is moved by this button. It's okay for \c control to be 0, in which case this button will simply do nothing until you call setControl() again with a valid control.
	void setControlR(AMControl* controlR);
	/// Returns the first control that is moved by this button. Could be 0 if this button has no control set.
	AMControl* control2() const { return control2_; }
	/// Returns the control that is moved by this button. Could be 0 if this button has no control set.
	AMControl* controlR() const { return controlR_; }
	/// Returns true if the move direction should be opposite the signs given in stepSizes().
	bool direction2Reversed() const { return directionReversed_; }


public slots:

	/// Specifies to reverse the direction of the moves.
	void setDirection2Reversed(bool trueForReverse) { direction2Reversed_ = trueForReverse; }

protected slots:
	/// When the button is clicked, relativeMove()s will be initiated on control() and control2(), based on distance of currentStepSize() and controlR().value().
	virtual void onButtonClicked();
	/// When the existing control1 is destroyed, we must set our control to 0.
	void onControl2Destroyed();
	/// When the existing controlR is destroyed, we must set our control to 0.
	void onControlRDestroyed();
	/// Checkes all controls and updates the button state
	void checkStatus();
	/// Called when the button is right-clicked to request a custom context menu
	void onCustomContextMenuRequested(const QPoint& point);



protected:
	AMControl* control2_;
	AMControl* controlR_;
	AMCompositeControlMoveButtonContextMenu* directionMoveContextMenu_;

	bool direction2Reversed_;

};

/// This widget is used as a custom context menu for AMControlMoveButton. It lets the user choose from a range of step sizes, and provides an AMControlEditor for viewing and setting the absolute position.
class AMCompositeControlMoveButtonContextMenu : public QDialog {
	Q_OBJECT
public:
	virtual ~AMCompositeControlMoveButtonContextMenu();
	AMCompositeControlMoveButtonContextMenu(AMControlMoveButton* moveButton);

protected slots:
	void onStepSizeIndexChanged(int newIndex);

protected:
	void leaveEvent(QEvent *);

	AMControlMoveButton* moveButton_;
	QButtonGroup stepSizeButtons_;
};



#endif // AMCOMPOSITECONTROLDIRECTIONMOVEBUTTON_H



