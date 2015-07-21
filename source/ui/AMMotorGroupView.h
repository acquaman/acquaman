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
#include "beamline/AMMotorGroup.h"

class AMMotorGroupObjectView : public QWidget
{
	Q_OBJECT

public:
	explicit AMMotorGroupObjectView(
			AMMotorGroupObject* motorGroupObject,
			QWidget* parent = 0);

	virtual ~AMMotorGroupObjectView() {}

protected slots:


protected:
	QLabel* titleLabel_;

};



class AMMotorGroupView : public QWidget
{
	Q_OBJECT
public:
	explicit AMMotorGroupView(AMMotorGroup* motorGroup_, QWidget* parent = 0);

	virtual ~AMMotorGroupView() {}

protected:
	QTabWidget* groupObjectTabs_;
};

///// This widget handles building a view for an AMMotorGroupObject.  Builds the view based on all the information provided in the object.
//class AMMotorGroupObjectView : public QWidget
//{
//	Q_OBJECT

//public:
//	/// Constructor.  Builds a view based around the provided \param motorGroupObject.
//	explicit AMMotorGroupObjectView(AMMotorGroupObject *motorGroupObject, QWidget *parent = 0);

//	virtual ~AMMotorGroupObjectView();

//	/// Returns the AMMotorGroupObject that this view encapsulates.
//	AMMotorGroupObject *motorGroupObject() const { return motorGroupObject_; }
//	/// Returns whether the title is visible.  There may be cases when stacking many motors together where the default name placement is not necessary.
//	bool titleIsVisible() const { return titleLabel_->isVisible(); }
//	/// Sets whether the title is visible.  There may be cases when stacking many motors together where the default name placement is not necessary.
//	void setTitleVisible(bool visible) { titleLabel_->setVisible(visible); }

//	/// Sets the precision (number of decimals) for the control setpoint boxes (defaults to 3)
//	void setControlSetpointPrecision(int controlSetpointsPrecision);

//	/// Returns a pointer to the jog spin box in case you need to change it's properties
//	QDoubleSpinBox *jogSpinBox();

//	/// Returns the list of pointers to the control setpoint spin boxes in case you need to change their properties
//	QList<QDoubleSpinBox *> controlSetpointsSpinBoxes();

//protected slots:
//	/// Slot that handles going up.
//	virtual void onUpClicked();
//	/// Slot that handles going down.
//	virtual void onDownClicked();
//	/// Slot that handles going left.
//	virtual void onLeftClicked();
//	/// Slot that handles going right.
//	virtual void onRightClicked();
//	/// Slot that handles going in.
//	virtual void onInClicked();
//	/// Slot that handles going out.
//	virtual void onOutClicked();
//	/// Slot that handles stopping.
//	void onStopClicked();
//	/// Slot that handles the first motor setpoint.
//	virtual void onFirstControlSetpoint();
//	/// Slot that handles the second motor setpoint.
//	virtual void onSecondControlSetpoint();
//	/// Slot that handles the third motor setpoint.
//	virtual void onThirdControlSetpoint();

//	/// Handles changes in the moving flags of the sample stages.
//	virtual void onMovingChanged();
//	/// Handles if the motors time out.
//	void onTimedOut() { setEnabled(false); }

//protected:
//	/// Holds the current motor group info pointer.
//	AMMotorGroupObject *motorGroupObject_;

//	/// Holds the title label.
//	QLabel *titleLabel_;
//	/// List that holds the prefix labels.
//	QList<QLabel *> prefixLabels_;
//	/// List that holds the control setpoint spin boxes.
//	QList<QDoubleSpinBox *> controlSetpoints_;
//	/// List that holds the small horizontal layouts containing the controlSetpoints
//	QList<QHBoxLayout*> controlSetpointLayouts_;
//	/// Holds the jog spin box.
//	QDoubleSpinBox *jog_;
//	/// Holds the status label
//	QLabel *status_;

//	/// Holds the precision (number of decimals) that will be displayed for the control setpoint boxes. Default is 3.
//	int controlSetpointsPrecision_;

//	// Buttons.
//	/// Stop button.
//	QToolButton *stopButton_;
//	/// The jog up button.
//	QToolButton *goUp_;
//	/// The jog down button.
//	QToolButton *goDown_;
//	/// The jog left button.
//	QToolButton *goLeft_;
//	/// The jog right button.
//	QToolButton *goRight_;
//	/// The jog in button.
//	QToolButton *goIn_;
//	/// The jog out button.
//	QToolButton *goOut_;

//	QGridLayout *arrowLayout_;
//	QHBoxLayout *motorGroupLayout_;
//	QVBoxLayout *absoluteValueLayout_;
//	QHBoxLayout *jogLayout_;
//};

///// This widget handles all of the AMMotorGroupObjectViews.
///*!
//	This widget has two viewing modes:  Single and Multiple.
//	The idea for this is that there may be cases where you only want to have
//	access to a single motor group object and there are times when you want
//	to show multiple at once.  To help facilitate that there are two different
//	basic views.

//	For Exclusive view, the title is actually a combo box where your selection dictates
//	which single view will be visible.  The right click menu will be considerably
//	smaller, containing only the ability to switch between modes.  There is a
//	convenience getter for "current view" that can return which view is currently
//	being viewed.  It will also emit a signal too this effect.

//	For Multiple view, the title is a standard label, but now the right click menu
//	contains all of the possible motor group objects that can be viewed.  If a particular
//	object is visible then the item will be checked off inside the list.  If any view
//	is to be hidden again, selecting that option again will suffice.  This view has
//	some convenience getters by returning the names or motor group objects of all
//	visible motor group objects.

//	\note The convenience getters will return default, null, or empty values when
//	not in the mode of which they pertain.
//  */
//class AMMotorGroupView : public QWidget
//{
//	Q_OBJECT

//public:
//	/// Enum for the two view modes.
//	enum ViewMode { Exclusive, Multiple };

//	/// Constructor.  Handles and builds all the views necessary for \param motorGroup.  Defaults to Exclusive view mode.
//	explicit AMMotorGroupView(AMMotorGroup *motorGroup, QWidget *parent = 0);
//	/// Constructor.  Handles and builds all the views necessary for \param motorGroup, also defines which view mode should be used.
//	explicit AMMotorGroupView(AMMotorGroup *motorGroup, ViewMode viewMode, QWidget *parent = 0);

//	virtual ~AMMotorGroupView();

//	/// Returns the view mode of this widget.
//	ViewMode viewMode() const { return viewMode_; }

//	/// Returns the name of the current AMMotorGroupObjectView.  Returns an empty string if in Multiple mode.
//	QString currentMotorGroupObjectName() const;
//	/// Returns the current AMMotorGroupObjectView.  Returns 0 if in multiple mode.
//	AMMotorGroupObjectView *currentMotorGroupObjectView() const;

//	/// Returns the names of all currently visible AMMotorGroupObjectViews.  Returns an empty list if in Exclusive Mode.
//	QStringList visibleMotorGroupObjectNames() const;
//	/// Returns a list of all of the currently visible AMMotorGroupObjectViews.  Returns an empty list if in Exclusive mode.
//	QList<AMMotorGroupObjectView *> visibleMotorGroupObjectViews() const;

//signals:
//	/// Notifier that the current motor group object view has changed.  Passes the name of the object.  Only emitted when using the exclusive view.
//	void currentMotorGroupObjectViewChanged(const QString &name);
//	/// Notifier that the list of motor group object views has changed.  Passes the name of the object that was recently changed or is empty in the case of multiple objects being changed simultaneously (such as a view mode change).  Only emitted when using the multiple view.
//	void motorGroupVisibilityChanged(const QString &name);

//public slots:
//	/// Sets the view associated with a given name.  This is a virtual method because this is likely the method that would be overloaded in subclasses.
//	virtual void setMotorGroupView(const QString &name);
//	/// Sets the view mode.  Changes necessary items to ensure view modes are consistent with constructed functionality.
//	void setViewMode(ViewMode mode);
//	/// Show/Hide availableMotorGroupObjects_
//	void showAvailableMotorGroupChoices(bool show);

//protected slots:
//	/// Handles the popup menu that allows you to change the motors you want to change.
//	virtual void onCustomContextMenuRequested(const QPoint &pos);

//protected:
//	/// Adds the standard actions to the popup menu based on its current view mode.
//	void buildStandardMenuItems(QMenu *menu);
//	/// Handles the standard actions from this class.
//	void handleStandardMenuItems(const QString &command);

//	/// Holds the view mode flag.
//	ViewMode viewMode_;
//	/// Holds the motor group pointer.
//	AMMotorGroup *motorGroup_;
//	/// Holds the map of views of motor group objects.
//	QMap<QString, AMMotorGroupObjectView *> motorGroupViews_;

//	/// Holds the current motor group object view and name as a QPair.
//	AMMotorGroupObjectView *currentMotorGroupObjectView_;
//	/// The combo box used to switch between available motor group objects when using the Exclusive view mode.
//	QComboBox *availableMotorGroupObjects_;
//	/// Holds the current list visible motor group objects and their visibility.  Used only for Multiple view.
//	QMap<QString, AMMotorGroupObjectView *> visibleMotorGroupObjectViews_;

//private:
//	/// The common function to initialize and layout the components of the MotorGroupView
//	void initAndLayoutMotorGroupView(AMMotorGroup *motorGroup, ViewMode viewMode);
//};

#endif // AMMOTORGROUPVIEW_H
