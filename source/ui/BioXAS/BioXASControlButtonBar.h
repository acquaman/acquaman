#ifndef BIOXASCONTROLBUTTONBAR_H
#define BIOXASCONTROLBUTTONBAR_H

#include <QMap>

#include "ui/BioXAS/BioXASButtonBar.h"

class AMControl;

class BioXASControlButtonBar : public BioXASButtonBar
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASControlButtonBar(QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASControlButtonBar();

	/// Returns the selected control.
	AMControl* selectedControl() const { return selectedControl_; }

signals:
	/// Notifier that the selected control has changed.
	void selectedControlChanged(AMControl *newControl);

public slots:
	/// Adds a button for the given control to the button bar.
	void addControl(AMControl *control, double greenValue);
	/// Removes the button for the given control.
	void removeControl(AMControl *control);
	/// Clears the buttons for all controls.
	void clearControls();

protected slots:
	/// Sets the selected control.
	void setSelectedControl(AMControl *control);
	/// Handles updating the selected control, the control corresponding to the selected button.
	void updateSelectedControl();

protected:
	/// Creates and returns a button for the given control.
	virtual QAbstractButton* createButton(AMControl *control, double greenValue);

protected:
	/// The selected control.
	AMControl *selectedControl_;
	/// The control button map.
	QMap<AMControl*, QAbstractButton*> controlButtonMap_;
};

#endif // BIOXASCONTROLBUTTONBAR_H
