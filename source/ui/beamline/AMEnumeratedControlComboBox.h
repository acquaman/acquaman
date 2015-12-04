#ifndef AMENUMERATEDCONTROLCOMBOBOX_H
#define AMENUMERATEDCONTROLCOMBOBOX_H

#include <QWidget>
#include <QComboBox>
#include <QLayout>

class AMEnumeratedControl;

class AMEnumeratedControlComboBox : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMEnumeratedControlComboBox(AMEnumeratedControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMEnumeratedControlComboBox();

	/// Returns the control being viewed.
	AMEnumeratedControl* control() const { return control_; }

signals:
	/// Notifier that the control being viewed has changed.
	void controlChanged(AMEnumeratedControl *newControl);

public slots:
	/// Refreshes the view.
	void refresh();

	/// Sets the control being viewed.
	void setControl(AMEnumeratedControl *newControl);

protected slots:
	/// Clears the combo box.
	void clearBox();
	/// Updates the combo box with the current control value.
	void updateBoxValue();

protected:
	/// The control being viewed.
	AMEnumeratedControl *control_;
	/// The combo box.
	QComboBox *comboBox_;
};

#endif // AMENUMERATEDCONTROLCOMBOBOX_H
