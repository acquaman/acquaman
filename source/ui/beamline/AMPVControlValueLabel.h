#ifndef AMPVCONTROLVALUELABEL_H
#define AMPVCONTROLVALUELABEL_H

#include <QWidget>
#include <QLabel>
#include <QLayout>

class AMReadOnlyPVControl;

class AMPVControlValueLabel : public QWidget
{
	Q_OBJECT
public:
	explicit AMPVControlValueLabel(AMReadOnlyPVControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMPVControlValueLabel();

	/// Returns the control being viewed.
	AMReadOnlyPVControl* control() const { return control_; }
	/// Returns the value label format.
	QChar format() const { return format_; }
	/// Returns the value label precision.
	int precision() const { return precision_; }

signals:
	/// Notifier that the control being viewed has changed.
	void controlChanged(AMReadOnlyPVControl *newControl);
	/// Notifier that the value label format has changed.
	void formatChanged(const QChar &newFormat);
	/// Notifier that the value label precision has changed.
	void precisionChanged(int newPrecision);

public slots:
	/// Refreshes the view.
	void refresh();

	/// Sets the value label format.
	void setFormat(const QChar &newFormat);
	/// Sets the value label precision.
	void setPrecision(int newPrecision);
	/// Sets the control being viewed.
	void setControl(AMReadOnlyPVControl *newControl);
	/// Sets the control being viewed, in addition to setting the format and precision.
	void setControl(AMReadOnlyPVControl *newControl, const QChar &newFormat, int newPrecision);

protected:
	/// The control being viewed.
	AMReadOnlyPVControl *control_;
	/// The value label.
	QLabel *valueLabel_;
	/// The value label format.
	QChar format_;
	/// The value label precision.
	int precision_;
};

#endif // AMPVCONTROLVALUELABEL_H
