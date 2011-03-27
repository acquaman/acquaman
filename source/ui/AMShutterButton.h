#ifndef AMSHUTTERBUTTON_H
#define AMSHUTTERBUTTON_H

#include <QPushButton>
#include <QRadialGradient>

#include "beamline/AMProcessVariable.h"

class AMShutterButton : public QPushButton
{
	Q_OBJECT
public:
	/// Enumerator used for the different states of the button.
	enum State { Open, Between, Closed };

	/// The reason for having two shutters is because not all types of shutters have remote control.  Safety Shutters (SSH) must be pushed on the ACIS panel.  Whereas Photon Shutters (PSH) can be control remotely through PV's.
	/// Constructor for the shutter button.  Requires the PV for the shutter state and the title that will be displayed on it.  Uses the CLS code for states: OPEN, BETWEEN, CLOSED.
	AMShutterButton(QString title, QString statusPV, QWidget *parent = 0);
	/// Constructor for opening the shutter and closing it.
	AMShutterButton(QString title, QString statusPV, QString openPV, QString closePV, QWidget *parent = 0);

signals:

public slots:

protected:
	/// Re-implemented paint function.
	virtual void paintEvent(QPaintEvent *e);

private slots:
	/// Initialize the pv.
	void statusUpdate();
	/// Toggles the state of the Shutter.  If Open -> Closed, if Closed -> Open.  Note that Between is reserved for beamline errors only.
	void changeState();

private:
	/// Returns the state of the Shutter.
	State state() const { return state_; }
	/// Sets the state.
	void setState(State state) { state_ = state; update(); }
	/// Returns the title.
	QString title() const { return title_; }

	// The state, pv, and title of the button.
	State state_;
	AMProcessVariable *statePV_;
	QString title_;
	AMProcessVariable *openPV_;
	AMProcessVariable *closePV_;
};

#endif // AMSHUTTERBUTTON_H
