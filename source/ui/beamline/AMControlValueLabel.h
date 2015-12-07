#ifndef AMCONTROLVALUELABEL_H
#define AMCONTROLVALUELABEL_H

#include <QWidget>
#include <QLabel>
#include <QLayout>

class AMControl;
class AMEnumeratedControlValueLabel;

class AMControlValueLabel : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMControlValueLabel(AMControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMControlValueLabel();

	/// Returns the control being viewed.
	AMControl* control() const { return control_; }
	/// Returns the value label format.
	QChar format() const { return format_; }
	/// Returns the value label precision.
	int precision() const { return precision_; }

signals:
	/// Notifier that the control being viewed has changed.
	void controlChanged(AMControl *newControl);
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
	/// Sets the value label stylesheet.
	void setStyleSheet(const QString &styleSheet);
	/// Sets the control being viewed.
	void setControl(AMControl *newControl);
	/// Sets the control being viewed, in addition to setting the format and precision.
	void setControl(AMControl *newControl, const QChar &newFormat, int newPrecision);

protected:
	/// The control being viewed.
	AMControl *control_;
	/// The value label.
	QLabel *valueLabel_;
	/// The value label format.
	QChar format_;
	/// The value label precision.
	int precision_;
	/// The enumerated control label.
	AMEnumeratedControlValueLabel *enumeratedValueLabel_;
};

#endif // AMCONTROLVALUELABEL_H
