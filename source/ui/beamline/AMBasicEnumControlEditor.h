#ifndef AMBASICENUMCONTROLEDITOR_H
#define AMBASICENUMCONTROLEDITOR_H

#include <QToolButton>
#include <QIcon>

class AMControl;

/// This widget displays the current state of an AMControl with enum values, and provides a drop-down menu to let the user request changes. The entries in the drop-down menu are based on the setpoint enum values, if AMControl::isEnum() returns true.
class AMBasicEnumControlEditor : public QToolButton
{
    Q_OBJECT
public:
	/// Constructor requires a valid \c control to show.
	AMBasicEnumControlEditor(AMControl* control, QWidget *parent = 0);

signals:

public slots:

protected slots:
	// Responding to changes from the control
	/////////////////////////////////////////

	/// Called when the control connects or disconnects
	void onControlConnected(bool connected);
	/// Called when the control value changes
	void onControlValueChanged(double value);
	/// Called when the control issues an enumChanges() signal.
	void onEnumChanges(const QStringList enumStates);
	/// Called when the moving status of the control changes
	void onControlMovingChanged(bool isMoving);

	// Responding to changes from the GUI
	/////////////////////////////
	/// Called when a user chooses an enum option from the drop-down menu to request a new setpoint.
	void onMenuActionTriggered(QAction* action);


	/// Called when a new value is selected from the enum drop-down menu. This is virtual so that subclasses can implement the move however they want. The default implementation just does an AMControl::move() to that value.
	virtual void onNewEnumValueChosen(int value);


protected:
	AMControl* control_;
	QMenu* menu_;
	QIcon movingIcon_;
};

#endif // AMBASICENUMCONTROLEDITOR_H
