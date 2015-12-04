#ifndef AMCONTROLCOMBOBOX_H
#define AMCONTROLCOMBOBOX_H

#include <QWidget>
#include <QLayout>

class AMControl;
class AMPVControlComboBox;
class AMEnumeratedControlComboBox;

class AMControlComboBox : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMControlComboBox(AMControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMControlComboBox();

	/// Returns the control being viewed.
	AMControl* control() const { return control_; }

signals:
	/// Notifier that the control being viewed has changed.
	void controlChanged(AMControl *newControl);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the control being viewed.
	void setControl(AMControl *newControl);

protected:
	/// The control being viewed.
	AMControl *control_;
	/// The PV control-specific view.
	AMPVControlComboBox *pvComboBox_;
	/// The enumerated pseudomotor control-specific view.
	AMEnumeratedControlComboBox *pseudoMotorComboBox_;
};

#endif // AMCONTROLCOMBOBOX_H
