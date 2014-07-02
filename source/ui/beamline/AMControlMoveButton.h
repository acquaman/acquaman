/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AMCONTROLMOVEBUTTON_H
#define AMCONTROLMOVEBUTTON_H

#include <QToolButton>
#include <QList>
#include <QDialog>
#include <QButtonGroup>

class AMControl;
class AMControlMoveButtonContextMenu;

/// This button widget allows you to move a control (AMControl) by a relative amount in a predefined direction with just a click. You can adjust the move distance by right-clicking to open a second panel, which also lets you see and set the absolute position. Future version will support continuous motion by holding the button down.  Two of these buttons provides a sufficient interface for things like sample positioners, etc.
class AMControlMoveButton : public QToolButton
{
	Q_OBJECT
public:
	/// Create a move button for the given \c control. The available move step sizes (in the control's units) are given by \c stepSizes.  If you specify \c reverseDirection, the direction of the move will be opposite of the sign of the values in stepSizes.  It's okay for \c control to be 0; this button will simply do nothing (and be disabled) until you call setControl() with a valid control.  This widget does not take ownership of the control.
 	virtual ~AMControlMoveButton();
	explicit AMControlMoveButton(QWidget *parent = 0,
								 AMControl* control = 0,
								 QList<double> stepSizes = QList<double>() << 1 << 10 << 100,
								 bool directionReversed = false);

	/// Set the control that is moved by this button. It's okay for \c control to be 0, in which case this button will simply do nothing until you call setControl() again with a valid control.
	void setControl(AMControl* control);
	/// Returns the control that is moved by this button. Could be 0 if this button has no control set.
	AMControl* control() const { return control_; }

	/// Returns the distance of the move step that will be taken when the button is clicked.
	double currentStepSize() const { return stepSizes_.at(stepSizeIndex_); }
	/// Returns the available step sizes that users can choose from with the right-click panel.
	QList<double> stepSizes() const { return stepSizes_; }
	/// Returns the index in stepSizes() of the current step size.
	int stepSizeIndex() const {return stepSizeIndex_; }

	/// Returns true if the move direction should be opposite the signs given in stepSizes().
	bool directionReversed() const { return directionReversed_; }

public slots:
	/// Select one of the available step sizes, from the list of stepSizes().
	bool setStepSizeIndex(int index);
	/// Sets the available step sizes that can be chosen (and are shown in the right-click panel). Returns false and does nothing if empty. The current stepSizeIndex() is reset to 0.
	bool setStepSizes(const QList<double>& stepSizes);

	/// Specifies to reverse the direction of the moves.
	void setDirectionReversed(bool trueForReverse) { directionReversed_ = trueForReverse; }

	/// "Press" the button: When set to true, the button's down state is activated.  When changed to false, the button is changed to the up state, and a click() is generated.  \note If the button is already in the up state when press(false) is called, no click() will be generated.
	void press(bool down = true);
	/// Synonym for press(false)
	void release() { press(false); }

signals:
	/// Emitted when the step size changes, with the index in stepSizes(). Used by the right-click panel AMControlMoveButtonContextMenu.
	void stepSizeIndexChanged(int newIndex);

protected slots:
	/// When the button is clicked, a relativeMove() will be initiated on the control(), with a distance of currentStepSize().  If reverseDirection() is true, the direction will be reversed from the sign of currentStepSize().
	void onButtonClicked();
	/// When the existing control is destroyed, we must set our control to 0.
	void onControlDestroyed();
	/// Called when the button is right-clicked to request a custom context menu
	void onCustomContextMenuRequested(const QPoint& point);

protected:

	AMControl* control_;
	QList<double> stepSizes_;
	int stepSizeIndex_;
	AMControlMoveButtonContextMenu* contextMenu_;
	bool directionReversed_;

};


/// This widget is used as a custom context menu for AMControlMoveButton. It lets the user choose from a range of step sizes, and provides an AMControlEditor for viewing and setting the absolute position.
class AMControlMoveButtonContextMenu : public QDialog {
	Q_OBJECT
public:
 	virtual ~AMControlMoveButtonContextMenu();
	AMControlMoveButtonContextMenu(AMControlMoveButton* moveButton);

protected slots:
	void onStepSizeIndexChanged(int newIndex);

protected:
	void leaveEvent(QEvent *);

	AMControlMoveButton* moveButton_;
	QButtonGroup stepSizeButtons_;
};

#endif // AMCONTROLMOVEBUTTON_H
