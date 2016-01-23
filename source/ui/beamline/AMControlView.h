#ifndef AMCONTROLVIEW_H
#define AMCONTROLVIEW_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QLayout>

#include "beamline/AMControl.h"

class AMControlView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMControlView(AMControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMControlView();

	/// Returns the control being viewed.
	AMControl* control() const { return control_; }

signals:
	/// Notifier that the control being viewed has changed.
	void controlChanged(AMControl*);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the control being viewed.
	void setControl(AMControl *newControl);

protected slots:
	/// Updates the name label.
	void updateNameLabel();
	/// Updates the description label.
	void updateDescriptionLabel();
	/// Updates the connected label.
	void updateConnectedLabel();
	/// Updates the value label.
	void updateValueLabel();
	/// Updates the value minimum label.
	void updateMinimumLabel();
	/// Updates the value maximum label.
	void updateMaximumLabel();
	/// Updates the values view.
	void updateValuesView();

protected:
	/// The control being viewed.
	AMControl *control_;

	/// The name label.
	QLineEdit *nameLabel_;
	/// The description label.
	QLineEdit *descriptionLabel_;
	/// The connected label.
	QLineEdit *connectedLabel_;
	/// The value label.
	QLineEdit *valueLabel_;
	/// The value minimum label.
	QLineEdit *valueMinimumLabel_;
	/// The value maximum label.
	QLineEdit *valueMaximumLabel_;
	/// The values prompt.
	QLabel *valuesPrompt_;
	/// The enum values view.
	QTextEdit *valuesView_;
};

#endif // AMCONTROLVIEW_H
