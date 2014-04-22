/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef VESPERSMOTORVIEW_H
#define VESPERSMOTORVIEW_H

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QMessageBox>

#include "beamline/AMPVControl.h"

class VESPERSMotorView : public QWidget
{
	Q_OBJECT

	/*!
	  This class is used to build the motor control screen for the Endstation class.  This class will handle all the relationships between the class
	  and the control.  The constructor primarily sets up the layout for the class, where as the \code setControl(AMControl *) function associates
	  the control aspsects with line edits and the like.

	  There are two types of \setControl(AMControl *): one that is a general full access control and one that is meant as a simple interface.  Since
	  more things are done for you with the simple interface, that function requires more information to work correctly.
	  */
public:
	/// Constructor.  Builds the layout.
 	virtual ~VESPERSMotorView();
	explicit VESPERSMotorView(QWidget *parent = 0);

	/// This function sets all the aspects of the control to the widget with the default full layout.  Assigns units of mm if the control doesn't have units.
	void setControl(AMControl *control);
	/// Overloaded function.  Same as \code setControl(AMControl *) but with soft limits.  Assigns units of mm if the control doesn't have units.
	void setControl(AMControl *control, double lowLimit, double highLimit);
	/// Overloaded function.  Sets all the aspects of the control to the widget with the simple layout.  Distances and labels for the two positions are required.
	void setControl(AMControl *control, double firstSetpoint, double secondSetpoint, QString firstLabel, QString secondLabel);
	/// Returns the current control.
	AMControl *control() const { return control_; }

signals:

public slots:

private slots:
	/// This slot updates the setpoint of the motor.
	void updateSetpoint(double setpoint);
	/// This slot updates the status of the motor.
	void updateStatus(int status);
	/// This slot moves the control.
	void move()
	{
		if (acceptableMove(setpoint_->text().toDouble()))
			control_->move(setpoint_->text().toDouble());
		else
			QMessageBox::warning(this, control_->name(), tr("The chosen setpoint is invalid.  Setpoints must be between %1 %2 and %3 %2.").arg(QString::number(lowLimit_)).arg(control_->units()).arg(QString::number(highLimit_)));
	}
	/// Overloaded function.  Moves to the specified point.
	void move(double setpoint)
	{
		if (acceptableMove(setpoint))
			control_->move(setpoint);
		else
			QMessageBox::warning(this, control_->name(), tr("The chosen setpoint is invalid.  Setpoints must be between %1 %2 and %3 %2.").arg(QString::number(lowLimit_)).arg(control_->units()).arg(QString::number(highLimit_)));
	}
	/// Jogs the control left.
	void jogLeft() { move(control_->value()-jog_->text().toDouble()); }
	/// Jogs the control right.
	void jogRight() { move(control_->value()+jog_->text().toDouble()); }
	/// Used for simple interface.  Moves to first setpoint.
	void moveToFirstSetpoint() { move(firstSetpoint_); }
	/// Used for simple interface.  Moves to the second setpoint.
	void moveToSecondSetpoint() { move(secondSetpoint_); }

private:
	/// Checks the new setpoint against the soft limits.
	bool acceptableMove(double setpoint) { if (lowLimit_ == highLimit_) return true; return (setpoint >= lowLimit_ && setpoint <= highLimit_) ? true : false; }
	/// Hides and disconnects the various elements from the main widget.
	void hideAndDisconnect();

	// Class members.
	enum Type { Full, Simple };

	// A pointer to the control.
	AMControl *control_;
	// The title of the control.
	QLabel *title_;
	// The control status label.
	QLabel *status_;
	// The setpoint line edit.
	QLineEdit *setpoint_;
	// The setpoint label.
	QLabel *setpointLabel_;
	// The jog line edit.
	QLineEdit *jog_;
	// The jog label.
	QLabel *jogLabel_;
	// The units label and jog units label.
	QLabel *units_, *jogUnits_;
	// The jog buttons.
	QToolButton *jogLeft_, *jogRight_;
	// The two setpoint buttons when the simple layout is used.
	QToolButton *firstSetpointButton_, *secondSetpointButton_;
	// The stop button.
	QToolButton *stop_;
	// Flag for determining which layout we need.
	Type type_;

	// Used to store the first and second setpoints in the simple interface.
	double firstSetpoint_, secondSetpoint_;
	// Used for soft limits.
	double lowLimit_, highLimit_;
};

#endif // VESPERSMOTORVIEW_H
