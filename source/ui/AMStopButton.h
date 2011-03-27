#ifndef AMSTOPBUTTON_H
#define AMSTOPBUTTON_H

#include <QToolButton>
#include "beamline/AMControl.h"

class AMStopButton : public QToolButton
{
	Q_OBJECT

public:
	/// Constructor that doesn't require an AMControl.  This is purely a button with a stop sign icon.  The \code setControl(AMcontrol *) fucntion will be needed for this to become useful.
	AMStopButton(QWidget *parent = 0)
		: QToolButton(parent)
	{
		setIcon(QIcon(":/stop.png"));
		control_ = 0;
	}

	/// Default constructor.  Requires an AMControl and invokes that control's stop() command when clicked.
	AMStopButton(AMControl *control, QWidget *parent = 0)
		: QToolButton(parent)
	{
		setIcon(QIcon(":/stop.png"));
		control_ = control;
		connect(this, SIGNAL(clicked()), control_, SLOT(stop()));
	}

	/// This function changes which control stop is associated with the button.
	void setControl(AMControl *control)
	{
		if (control_)
			disconnect(this, SIGNAL(clicked()), control_, SLOT(stop()));
		control_ = control;
		connect(this, SIGNAL(clicked()), control_, SLOT(stop()));
	}

private:
	// Class members.
	// This is a pointer to the control.  Used to change controls.
	AMControl *control_;

};

class AMGroupStopButton : public QToolButton
{
	Q_OBJECT

public:
	/// Default constructor.  Requires a valid AMControl and will issue the control's stop() command when clicked.
	AMGroupStopButton(AMControl *control, QWidget *parent = 0)
		: QToolButton(parent)
	{
		setIcon(QIcon(":/stop.png"));
		connect(this, SIGNAL(clicked()), control, SLOT(stop()));
		controlList_ << control;
	}

	/// Overloaded constructor.  Can give a list of existing AMControls and will set them all up.
	AMGroupStopButton(QList<AMControl *> controlList, QWidget *parent = 0)
		: QToolButton(parent)
	{
		setIcon(QIcon(":/stop.png"));
		for (int i = 0; i < controlList.size(); i++)
			connect(this, SIGNAL(clicked()), controlList.at(i), SLOT(stop()));
		controlList_ << controlList;
	}

	/// Adds a control to the list of controls to be stopped by the button.  Connects the button the stop slot of the control.
	void addControl(AMControl *control) { controlList_ << control; connect(this, SIGNAL(clicked()), control, SLOT(stop())); }
	/// Overloaded function.  Adds a list of controls to the current list.
	void addControl(QList<AMControl *> list) { for (int i = 0; i < list.size(); i++) connect(this, SIGNAL(clicked()), list.at(i), SLOT(stop())); controlList_ << list; }
	/// Removes a control from the list of controls based on its ObjectName to be stopped by the button.  Removes the connection between the stop button and the slot of the control.
	void removeByName(AMControl *control)
	{
		for (int i = 0; i < controlList_.size(); i++) {

			if (QString::compare(controlList_.at(i)->name(), control->name()) == 0){

				controlList_.removeAt(i);
				break;
			}
		}

		disconnect(this, SIGNAL(clicked()), control, SLOT(stop()));
	}
	/// Overloaded funciton.  Removes all the controls from the list.
	void removeControl() { for (int i = 0; i < controlList_.size(); i++) disconnect(this, SIGNAL(clicked()), controlList_.at(i), SLOT(stop())); controlList_.clear(); }

private:
	QList<AMControl *> controlList_;
};

#endif // AMSTOPBUTTON_H
