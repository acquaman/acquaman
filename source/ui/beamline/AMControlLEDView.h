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
	void clear();
	/// Updates the view.
	void update();
	/// Refreshes the view.
	void refresh();

	/// Sets the control being viewed.
	void setControl(AMControl *newControl);

protected:
	/// The control being viewed.
	AMControl *control_;

	/// The LED.
	QLabel *led_;
};

#endif // AMCONTROLLEDVIEW_H
