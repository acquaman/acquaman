#ifndef BIOXASCONTROLBUTTON_H
#define BIOXASCONTROLBUTTON_H

#include "beamline/AMControl.h"
#include "dataman/AMNumber.h"
#include "ui/AMToolButton.h"

class BioXASControlButton : public AMToolButton
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASControlButton(AMControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASControlButton();

	/// Returns the control being viewed.
	AMControl* control() const { return control_; }

	/// Returns true if the green value is set, false otherwise.
	bool greenValueSet() const { return greenValueSet_; }
	/// Returns the green value.
	double greenValue() const { return greenValue_; }

signals:
	/// Notifier that the control being viewed.
	void controlChanged(AMControl *newControl);
	/// Notifier that the green value has changed.
	void greenValueChanged(double newValue);

public slots:
	/// Refreshes the view.
	void refresh();

	/// Sets the control being viewed.
	void setControl(AMControl *newControl);
	/// Sets the green value.
	void setGreenValue(double newValue);

protected:
	/// The control being viewed.
	AMControl *control_;

	/// Flag indicating whether the green value has been set.
	bool greenValueSet_;
	/// The green value.
	double greenValue_;
};

#endif // BIOXASCONTROLBUTTON_H
