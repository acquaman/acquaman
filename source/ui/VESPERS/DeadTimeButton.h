#ifndef DEADTIMEBUTTON_H
#define DEADTIMEBUTTON_H

#include <QToolButton>

class DeadTimeButton : public QToolButton
{
	/// This class is essentially meant to overide the paint event so that the only thing in the button is a color.  It accepts signals of new value and based on that value will change color.

	Q_OBJECT
public:
	/// Constructor.  Need to provide a number between 0 and 100 for the \param good and \param bad because they use those to determine the color.  The color is green if < good, yellow if good < x < bad, and red if > bad.
	DeadTimeButton(double good, double bad, QWidget *parent = 0);

	/// Returns the good setpoint.
	double good() const { return good_; }
	/// Returns the bad setpoint.
	double bad() const { return bad_; }
	/// Returns the current value.
	double current() const { return current_; }

public slots:
	/// Sets the good setpoint.
	void setGood(double val) { good_ = val; }
	/// Sets the bad setpoint.
	void setBad(double val) { bad_ = val; }
	/// Sets the current value.
	void setCurrent(double val) { current_ = val; }

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

#endif // DEADTIMEBUTTON_H
