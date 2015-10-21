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
	/// Clears the view.
	virtual void clear();
	/// Updates the view. Subclasses should reimplement for their desired behavior.
	virtual void update();
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

protected:
	/// The control being viewed.
	AMControl *control_;

	/// The LED.
	QLabel *led_;
};

#endif // AMCONTROLLEDVIEW_H
