#ifndef AMENUMERATEDCONTROLVALUELABEL_H
#define AMENUMERATEDCONTROLVALUELABEL_H

#include <QWidget>
#include <QLabel>
#include <QLayout>

class AMEnumeratedControl;

class AMEnumeratedControlValueLabel : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMEnumeratedControlValueLabel(AMEnumeratedControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMEnumeratedControlValueLabel();

	/// Returns the control being viewed.
	AMEnumeratedControl* control() const { return control_; }

signals:
	/// Notifier that the control being viewed has changed.
	void controlChanged(AMEnumeratedControl *newControl);

public slots:
	/// Refreshes the view.
	void refresh();

	/// Sets the value label stylesheet.
	void setStyleSheet(const QString &styleSheet);
	/// Sets the control being viewed.
	void setControl(AMEnumeratedControl *newControl);

protected:
	/// The control being viewed.
	AMEnumeratedControl *control_;
	/// The value label.
	QLabel *valueLabel_;
};

#endif // AMENUMERATEDCONTROLVALUELABEL_H
