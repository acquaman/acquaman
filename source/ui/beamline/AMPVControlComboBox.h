#ifndef AMPVCONTROLCOMBOBOX_H
#define AMPVCONTROLCOMBOBOX_H

#include <QWidget>
#include <QComboBox>
#include <QLayout>

class AMReadOnlyPVControl;

class AMPVControlComboBox : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMPVControlComboBox(AMReadOnlyPVControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMPVControlComboBox();

	/// Returns the control being viewed.
	AMReadOnlyPVControl* control() const { return control_; }

signals:
	/// Notifier that the control being viewed has changed.
	void controlChanged(AMReadOnlyPVControl *newControl);

public slots:
	/// Refreshes the view.
	void refresh();

	/// Sets the control being viewed.
	void setControl(AMReadOnlyPVControl *newControl);

protected slots:
	/// Clears the combo box.
	void clearBox();
	/// Updates the combo box value.
	void updateBoxValue();

protected:
	/// The control being viewed.
	AMReadOnlyPVControl *control_;
	/// The combo box.
	QComboBox *comboBox_;
};

#endif // AMPVCONTROLCOMBOBOX_H
