#ifndef AMCONTROLSTOPBUTTON_H
#define AMCONTROLSTOPBUTTON_H

#include <QWidget>
#include <QPushButton>
#include <QLayout>

class AMControl;

class AMControlStopButton : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMControlStopButton(AMControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMControlStopButton();

	/// Returns the control.
	AMControl* control() const { return control_; }

signals:
	/// Notifier that the control has changed.
	void controlChanged(AMControl *newControl);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the control.
	void setControl(AMControl *newControl);
	/// Sets the button's stylesheet.
	void setStyleSheet(const QString &styleSheet);

protected slots:
	/// Handles situation when the stop button is clicked.
	void onStopButtonClicked();

protected:
	/// The control being viewed.
	AMControl *control_;
	/// The stop button.
	QPushButton *stopButton_;
};

#endif // AMCONTROLSTOPBUTTON_H
