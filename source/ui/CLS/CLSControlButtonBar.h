#ifndef CLSCONTROLBUTTONBAR_H
#define CLSCONTROLBUTTONBAR_H

#include <QMap>

#include "ui/CLS/CLSButtonBar.h"

class AMControl;

class CLSControlButtonBar : public CLSButtonBar
{
	Q_OBJECT

public:
	/// Constructor.
	explicit CLSControlButtonBar(CLSButtonBar::SelectionMode selectionMode, QWidget *parent = 0);
	/// Destructor.
	virtual ~CLSControlButtonBar();

	/// Returns the selected control.
	AMControl* selectedControl() const { return selectedControl_; }

signals:
	/// Notifier that the selected control has changed.
	void selectedControlChanged(AMControl *newControl);
	/// Notifier that the button corresponding to a component has been clicked.
	void controlClicked(AMControl *control);

public slots:
	/// Adds a button for the given control to the button bar.
	void addControl(AMControl *control, double greenValue);
	/// Removes the button for the given control.
	void removeControl(AMControl *control);
	/// Clears the buttons for all controls.
	void clearControls();

	/// Sets the selected control.
	void setSelectedControl(AMControl *control);

protected slots:
	/// Handles updating the selected control, the control corresponding to the selected button.
	void updateSelectedControl();

	/// Handles updating the selected button, in response to a button being clicked. Reimplemented to provide notification that a control has been clicked.
	virtual void onButtonClicked(QAbstractButton *clickedButton);

protected:
	/// Creates and returns a button for the given control.
	virtual QAbstractButton* createButton(AMControl *control, double greenValue);

protected:
	/// The selected control.
	AMControl *selectedControl_;
	/// The control button map.
	QMap<AMControl*, QAbstractButton*> controlButtonMap_;
};

#endif // CLSCONTROLBUTTONBAR_H
