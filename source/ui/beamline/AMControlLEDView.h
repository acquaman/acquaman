#ifndef AMCONTROLLEDVIEW_H
#define AMCONTROLLEDVIEW_H

#include <QWidget>
#include <QLabel>
#include <QLayout>

#include "beamline/AMControl.h"
#include "dataman/AMNumber.h"

class AMControlLEDView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMControlLEDView(AMControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMControlLEDView();

	/// Returns the control being viewed.
	AMControl* control() const { return control_; }

signals:
	/// Notifier that the control being viewed has changed.
	void controlChanged(AMControl *newControl);

public slots:
	/// Refreshes the view, clears it then updates it.
	virtual void refresh();
	/// Sets the control being viewed.
	void setControl(AMControl *newControl);

protected slots:
	/// Sets the led as green, unlit.
	void setAsGreenOff();
	/// Sets the led as green, lit.
	void setAsGreenOn();

	/// Sets the led as red, unlit.
	void setAsRedOff();
	/// Sets the led as red, lit.
	void setAsRedOn();

	/// Sets the led as yellow, unlit.
	void setAsYellowOff();
	/// Sets the led as yellow, lit.
	void setAsYellowOn();

	/// Updates the LED label.
	virtual void updateLED();

protected:
	/// Returns true if the LED should currently be unlit and green.
	virtual bool shouldBeGreenOff() const { return false; }
	/// Returns true if the LED should currently be lit and green.
	virtual bool shouldBeGreenOn() const { return false; }

	/// Returns true if the LED should currently be unlit and red.
	virtual bool shouldBeRedOff() const { return false; }
	/// Returns true if the LED should currently be lit and red.
	virtual bool shouldBeRedOn() const { return false; }

	/// Returns true if the LED should currently be unlit and yellow.
	virtual bool shouldBeYellowOff() const { return false; }
	/// Returns true if the LED should currently be lit and yellow.
	virtual bool shouldBeYellowOn() const { return false; }

protected:
	/// The control being viewed.
	AMControl *control_;

	/// The LED label.
	QLabel *led_;
};

#endif // AMCONTROLLEDVIEW_H
