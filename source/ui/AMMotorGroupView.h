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


#ifndef AMMOTORGROUPVIEW_H
#define AMMOTORGROUPVIEW_H

#include <QWidget>
#include <QLabel>
#include <QToolButton>
#include <QTabWidget>
#include <QDoubleSpinBox>
#include <QTextEdit>
#include <QSignalMapper>
#include <QComboBox>
#include <QStackedWidget>
#include "beamline/AMMotorGroup.h"
#include "ui/beamline/AMExtendedControlEditor.h"

/*!
  * \brief A class for visualizing the data of an AMMotorGroupObject.
  * Allows the users to set the position of any of the axes within a motor group
  * object, as well as 'step' each of the axes by customizable amounts.
  */
class AMMotorGroupObjectView : public QWidget
{
	Q_OBJECT

public:
	/*!
	  * Creates an instance of an AMMotorGroupView, which will visualize the data
	  * contained within the provided motor group object.
	  */
	explicit AMMotorGroupObjectView(
			AMMotorGroupObject* motorGroupObject,
			QWidget* parent = 0);

	/*!
	  * Virtual destructor for an AMMotorGroupObjectView
	  */
	virtual ~AMMotorGroupObjectView() {}

public slots:
	/*!
	  * Sets the jog size value displayed in the jog spinbox.
	  * \param jogSize ~ The size of each jog to display in the jog spinbox
	  */
	void setJogSize(double jogSize);

	/*!
	  * Sets the single step size (ie. how much the jog size is incremented and
	  * decremented with a click) of the jog spin box.
	  * \param jogSingleStep ~ The size of a single increment/decrement of the jog
	  * value.
	  */
	void setJogSingleStep(double jogSingleStep);

	/*!
	  * Sets how many decimals of precision are to be displayed in the jog spin
	  * box.
	  * \param jogPrecision ~ The number of decimals to display in the jog spin
	  * box.
	  */
	void setJogPrecision(int jogPrecision);

	/*!
	  * Sets the min and max size of the values in the jog spin box.
	  * \parma minJog ~ The minimum value which can be entered in the jog spin
	  * box.
	  * \param maxJog ~ The maximum value which can be entered in the jog spin box.
	  */
	void setJogRange(double minJog, double maxJog);

	/*!
	  * Sets how many decimals of precision are to be displayed in the motor values
	  * setpoint spin boxes.
	  * \parma motorValuesPrecision ~ The precision to be displayed in the motor
	  * values spin box.
	  */
	void setMotorValuesPrecision(int motorValuesPrecision);

	/*!
	  * Sets the min and max values which can be entered into the motor values
	  * spinboxes.
	  * \param minValue ~ The minimum value which can be entered into the motor
	  * value spin boxes.
	  * \param maxValue ~ The maximum value which can be entered into the motor
	  * value spin boxes.
	  */
	void setMotorValuesRange(double minValue, double maxValue);

protected slots:
	/*!
	  * Handles signals indicating that a motor within the group object has
	  * had its connection state altered.
	  * \param direction ~ The direction of the motor.
	  * \param motionType ~ The motion type of the motor.
	  * \param isConnected ~ The connected state of the motor.
	  */
	void onConnectionStateChanged(AMMotorGroupObject::MotionDirection direction,
								  AMMotorGroupAxis::MotionType motionType,
								  bool isConnected);

	/*!
	  * Handles signals indicating that a motor within the group object has encountered
	  * an error.
	  * \param direction ~ The direction of the motor.
	  * \param motionType ~ The motion type of the motor.
	  * \param errorCode ~ The code which describes the type of error.
	  */
	void onMotorError(AMMotorGroupObject::MotionDirection direction,
					  AMMotorGroupAxis::MotionType motionType,
					  int errorCode);

	/*!
	  * Handles signals indicating that a motor within the group object has had
	  * its movement state altered.
	  * \param direction ~ The direction of the motor.
	  * \param motionType ~ The motion type of the motor.
	  * \param isMoving ~ The movement state of the motor.
	  */
	void onMovingStateChanged(AMMotorGroupObject::MotionDirection direction,
							  AMMotorGroupAxis::MotionType motionType,
							  bool isMoving);

	/*!
	  * Handles signals indicating that a motor within the group object has had
	  * its position units changed.
	  * \param direction ~ The direction of the motor.
	  * \param motionType ~ The motion type of the motor.
	  * \param positionUnits ~ The position units of the motor.
	  */
	void onPositionUnitsChanged(AMMotorGroupObject::MotionDirection direction,
								AMMotorGroupAxis::MotionType motionType,
								const QString& positionUnits);

	/*!
	  * Handles signals indicating that the increment horizontal translation button
	  * has been clicked. Moves the horizontal translation motor by the value
	  * speicifed in the step spin box, if the motor can be moved.
	  */
	void onHorizontalTranslationIncrementClicked();

	/*!
	  * Handles signals indicating that the decrement horizontal translation button
	  * has been clicked. Moves the horizontal translation motor backward by the value
	  * speicifed in the step spin box, if the motor can be moved.
	  */
	void onHorizontalTranslationDecrementClicked();

	/*!
	  * Handles signals indicating that the increment horizontal rotation button
	  * has been clicked. Moves the horizontal rotation motor by the value
	  * speicifed in the step spin box, if the motor can be moved.
	  */
	void onHorizontalRotationIncrementClicked();

	/*!
	  * Handles signals indicating that the decrement horizontal rotation button
	  * has been clicked. Moves the horizontal rotation motor backward by the value
	  * speicifed in the step spin box, if the motor can be moved.
	  */
	void onHorizontalRotationDecrementClicked();

	/*!
	  * Handles signals indicating that the increment vertical translation button
	  * has been clicked. Moves the vertical translation motor by the value
	  * speicifed in the step spin box, if the motor can be moved.
	  */
	void onVerticalTranslationIncrementClicked();

	/*!
	  * Handles signals indicating that the decrement vertical translation button
	  * has been clicked. Moves the horizontal translation motor backward by the value
	  * speicifed in the step spin box, if the motor can be moved.
	  */
	void onVerticalTranslationDecrementClicked();

	/*!
	  * Handles signals indicating that the increment vertical rotation button
	  * has been clicked. Moves the vertical rotation motor by the value
	  * speicifed in the step spin box, if the motor can be moved.
	  */
	void onVerticalRotationIncrementClicked();

	/*!
	  * Handles signals indicating that the decrement vertical rotation button
	  * has been clicked. Moves the vertical rotation motor backward by the value
	  * speicifed in the step spin box, if the motor can be moved.
	  */
	void onVerticalRotationDecrementClicked();

	/*!
	  * Handles signals indicating that the increment Normal translation button
	  * has been clicked. Moves the Normal translation motor by the value
	  * speicifed in the step spin box, if the motor can be moved.
	  */
	void onNormalTranslationIncrementClicked();

	/*!
	  * Handles signals indicating that the decrement Normal translation button
	  * has been clicked. Moves the horizontal translation motor backward by the value
	  * speicifed in the step spin box, if the motor can be moved.
	  */
	void onNormalTranslationDecrementClicked();

	/*!
	  * Handles signals indicating that the increment Normal rotation button
	  * has been clicked. Moves the Normal rotation motor by the value
	  * speicifed in the step spin box, if the motor can be moved.
	  */
	void onNormalRotationIncrementClicked();

	/*!
	  * Handles signals indicating that the decrement Normal rotation button
	  * has been clicked. Moves the Normal rotation motor backward by the value
	  * speicifed in the step spin box, if the motor can be moved.
	  */
	void onNormalRotationDecrementClicked();

	/*!
	  * Handles signals indicating that the stop button has been clicked. Instructs
	  * the motor group object to stop all of its axes.
	  */
	void onStopClicked();

	/*!
	  * Handles signals indicating that the dismiss error message button has been
	  * clicked.
	  */
	void onErrorMessageDismissClicked();

protected:

	/*!
	  * Helper method which performs all the initialization and layout of the
	  * contained widgets.
	  */
	void setupUi();

	/*!
	  * \brief Helper method which sets the data values and states for all the
	  * contained widgets based on the data within the motor group object model.
	  *
	  * This must be run after setupUi()
	  */
	void setupData();

	/*!
	  * \brief Helper method which connects the signals from the motor group object
	  * model to the slots within this view.
	  */
	void setupConnections();

	/*!
	  * Sets the position units to be displayed in the jog spinbox. Shows a set
	  * of each position unit contained within the group object, separated by a "/"
	  */
	void refreshJogUnits();

	/*!
	  * \brief Virtual implementation function which allows subclasses to perform
	  * customized actions when an error is encountered.
	  * In order to define such custom actions, subclass this class, then override
	  * this method.
	  * \param direction ~ The direction of the motor which encountered the error.
	  * \param motionType ~ The motion type of the motor which encountered the
	  * error.
	  * \param errorCode ~ A code which identified the type of error encountered
	  * by the motor.
	  */
	virtual void errorHandleImplementation(AMMotorGroupObject::MotionDirection direction,
										   AMMotorGroupAxis::MotionType motionType,
										   int errorCode);	

	// Model Reference
	AMMotorGroupObject* motorGroupObject_;


	// Widgets
	QLabel* errorLabel_;
	QLabel* statusLabel_;
	QToolButton* dismissError_;

	QToolButton* horizontalTranslationIncrement_;
	QToolButton* horizontalTranslationDecrement_;
	AMExtendedControlEditor* horizontalTranslationValue_;
	QLabel* horizontalTranslationLabel_;

	QToolButton* horizontalRotationIncrement_;
	QToolButton* horizontalRotationDecrement_;
	AMExtendedControlEditor* horizontalRotationValue_;
	QLabel* horizontalRotationLabel_;

	QToolButton* verticalTranslationIncrement_;
	QToolButton* verticalTranslationDecrement_;
	AMExtendedControlEditor* verticalTranslationValue_;
	QLabel* verticalTranslationLabel_;

	QToolButton* verticalRotationIncrement_;
	QToolButton* verticalRotationDecrement_;
	AMExtendedControlEditor* verticalRotationValue_;
	QLabel* verticalRotationLabel_;

	QToolButton* normalTranslationIncrement_;
	QToolButton* normalTranslationDecrement_;
	AMExtendedControlEditor* normalTranslationValue_;
	QLabel* normalTranslationLabel_;

	QToolButton* normalRotationIncrement_;
	QToolButton* normalRotationDecrement_;
	AMExtendedControlEditor* normalRotationValue_;
	QLabel* normalRotationLabel_;

	QDoubleSpinBox* jogSize_;
	QToolButton* stopAllButton_;

};


/*!
  * A class for visualizing the data within a motor group. Each motor group
  * object contained within the group will be visualized as a separate tab.
  */
class AMMotorGroupView : public QWidget
{
	Q_OBJECT
public:
	/*!
	  * Enumerates the different view modes for the AMMotorGroupView. Compact
	  * displays each contained motor group object view under a combo box, normal
	  * view displays each of them under a tab of a tab widget.
	  */
	enum ViewMode {
		NormalView,
		CompactView
	};

	/*!
	  * Creates an instance of an AMMotorGroupView which will visualize the data
	  * within the provided AMMotorGroup.
	  * \param motorGroup ~ The motor group whose data is to be visualized.
	  * \param viewMode ~ The viewmode to use to display the motor group.
	  */
	explicit AMMotorGroupView(AMMotorGroup* motorGroup,
							  ViewMode viewMode,
							  QWidget* parent = 0);

	/*!
	  * Virtual destructor for an AMMotorGroupView
	  */
	virtual ~AMMotorGroupView() {}

	/*!
	  * The current visible group object. If no group object is visible (eg if the
	  * group is empty) 0 is returned.
	  */
	AMMotorGroupObject* selectedGroupObject() const;

	/*!
	  * Sets the current visible group object view to the one which matches the
	  * passed group object name. If no group object is found with the provided
	  * name, then no change is made.
	  * \param groupObjectName ~ The name of the group object whose view is to
	  * be made visible.
	  */
	void setSelectedGroupObject(const QString& groupObjectName);

	/*!
	  * for some usage, we might NOT want the user knows the exists of the different motor groups
	  */
	void hideMotorGroupSelection();

signals:
	/*!
	  * Signal which indicates that the current motor group object view being
	  * displayed has altered.
	  * \param groupObjectName ~ The name of the group object whose view is being
	  * displayed.
	  */
	void currentMotorGroupObjectViewChanged(const QString& groupObjectName);

protected slots:
	/*!
	  * Handles signals indicating that one of the motors in one of the contained
	  * motor group objects has had its motion status altered.
	  * \param groupObjectName ~ The name of the group object which contains the
	  * motor whose motion status has been altered.
	  */
	void onGroupObjectMotionStatusAltered(const QString& groupObjectName);

	/*!
	  * Handles signals indicating that the group combo box has had its index
	  * changed. Sets the visible group object view to that of the selected index
	  * then emits the currentMotorGroupObjectViewChanged signal.
	  * \param index ~ The index of the combo box which was selected. Indicates
	  * the index in the stack widget which contains the view to show.
	  */
	void onGroupComboBoxIndexChanged(int index);

	/*!
	  * Handles signals indicating that the group tab view has had its selected
	  * tab index changed. Emits the currentMotorGroupObjectViewChanged signal.
	  * \param index ~ The index of the tab which was selected.
	  */
	void onGroupTabViewIndexChanged(int index);
protected:

	/*!
	  * Helper method which creates the ui for the compact view type
	  */
	void setupCompactUi();

	/*!
	  * Helper method which creates the ui for the normal view type
	  */
	void setupNormalUi();

	AMMotorGroup* motorGroup_;
	QString currentSelectedGroupObjectName_;
	ViewMode viewMode_;
	// Used only for normal view:
	QTabWidget* groupObjectTabs_;
	QHash<QString, int>	 motorGroupTabMap_;
	QSignalMapper* groupMotionStatusMapper_;

	// Used only for compact view:
	QComboBox* groupObjectSelector_;
	QStackedWidget* groupObjectStack_;

};
#endif // AMMOTORGROUPVIEW_H
