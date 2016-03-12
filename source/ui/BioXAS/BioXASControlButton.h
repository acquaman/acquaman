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
	/// Returns the green value.
	AMNumber greenValue() const { return greenValue_; }

signals:
	/// Notifier that the control being viewed.
	void controlChanged(AMControl *newControl);
	/// Notifier that the green value has changed.
	void greenValueChanged(const AMNumber &newValue);

public slots:
	/// Refreshes the view.
	void refresh();

	/// Sets the control being viewed.
	void setControl(AMControl *newControl);
	/// Sets the green value.
	void setGreenValue(const AMNumber &newValue);

protected:
	/// The control being viewed.
	AMControl *control_;
	/// The green value.
	AMNumber greenValue_;
};

#endif // BIOXASCONTROLBUTTON_H
