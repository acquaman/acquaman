#ifndef AMCONTROLTOOLBUTTON_H
#define AMCONTROLTOOLBUTTON_H

#include "ui/AMToolButton.h"

class AMControl;

class AMControlToolButton : public AMToolButton
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMControlToolButton(AMControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMControlToolButton();

	/// Returns the control.
	AMControl* control() const { return control_; }

signals:
	/// Notifier that the control has changed.
	void controlChanged(AMControl *newControl);

public slots:
	/// Refreshes the button.
	virtual void refresh();
	/// Sets the control.
	virtual void setControl(AMControl *newControl);

protected slots:
	/// Updates the background color.
	virtual void updateColor();

protected:
	/// Returns the (desired) current background color. Subclasses should reimplement for their particular color behavior.
	virtual QColor currentColor() const { return QColor(); }

protected:
	/// The control.
	AMControl *control_;
};

#endif // AMCONTROLTOOLBUTTON_H
