/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef VESPERSDEADTIMEBUTTON_H
#define VESPERSDEADTIMEBUTTON_H

#include <QToolButton>

class VESPERSDeadTimeButton : public QToolButton
{
	/// This class is essentially meant to overide the paint event so that the only thing in the button is a color.  It accepts signals of new value and based on that value will change color.

	Q_OBJECT
public:
	/// Constructor.  Need to provide a number between 0 and 100 for the \param good and \param bad because they use those to determine the color.  The color is green if < good, yellow if good < x < bad, and red if > bad.
	VESPERSDeadTimeButton(double good, double bad, QWidget *parent = 0);

	/// Returns the good setpoint.
	double good() const { return good_; }
	/// Returns the bad setpoint.
	double bad() const { return bad_; }
	/// Returns the current value.
	double current() const { return current_; }

public slots:
	/// Sets the good setpoint.
	void setGood(double val) { good_ = val; update(); }
	/// Sets the bad setpoint.
	void setBad(double val) { bad_ = val; update(); }
	/// Sets the current value.
	void setCurrent(double val) { current_ = val; update(); }

protected:
	/// Re-implemented paint event.
	void paintEvent(QPaintEvent *e);

	/// The good setpoint.
	double good_;
	/// The bad setpoint.
	double bad_;
	/// The current value.
	double current_;
};

#endif // VESPERSDEADTIMEBUTTON_H
