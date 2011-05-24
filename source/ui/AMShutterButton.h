#ifndef AMSHUTTERBUTTON_H
#define AMSHUTTERBUTTON_H

#include <QPushButton>
#include <QRadialGradient>

#include "beamline/AMProcessVariable.h"

/*!
	This class sets up connection to the necessary PVs to get a shutter to change state.  It contains the ability to change the state of the shutter, but must be manually invoked.
  */
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

	/// Returns the state of the Shutter.
	State state() const { return state_; }
	/// Returns the title.
	QString title() const { return title_; }

signals:

public slots:
	/// Opens the shutter.  Does nothing if the shutter is already open.
	void open() { if (state_ != Open) changeState(); }
	/// Closes the shutter.  Does nothing if the shutter is already closed.
	void close() { if (state_ != Closed) changeState(); }
	/// Toggles the state of the Shutter.  If Open -> Closed, if Closed -> Open.  Note that Between is reserved for beamline errors only.  Checks and only attempts to change state if it has the ability and has valid open and close PVs.
	void changeState();

protected:
	/// Re-implemented paint function.
	virtual void paintEvent(QPaintEvent *e);

private slots:
	/// Initialize the pv.
	void statusUpdate();

private:
	/// Sets the state.
	void setState(State state) { state_ = state; update(); }

	// The state, pv, and title of the button.
	State state_;
	AMProcessVariable *statePV_;
	QString title_;
	AMProcessVariable *openPV_;
	AMProcessVariable *closePV_;
};

#endif // AMSHUTTERBUTTON_H
