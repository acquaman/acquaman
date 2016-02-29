#ifndef BIOXASUTILITIESGROUP_H
#define BIOXASUTILITIESGROUP_H

#include "beamline/BioXAS/BioXASTriStateGroup.h"

class BioXASUtilitiesGroup : public BioXASTriStateGroup
{
	Q_OBJECT

public:
public:
	/// Enumeration of the possible value states.
	enum Value { Bad = 0, Good = 1 };

	/// Constructor.
	BioXASUtilitiesGroup(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASUtilitiesGroup();

	/// Returns true if this control is bad, false otherwise.
	virtual bool isBad() const;
	/// Returns true if this control is good, false otherwise.
	virtual bool isGood() const;

	/// Returns true if the given control is a child of the flow switches, false otherwise.
	bool hasControl(AMControl *control) const;

	/// Returns the list of flow switch controls.
	QList<AMControl*> controlsList() const { return children_; }
	/// Returns the list of flow switch controls that are bad.
	QList<AMControl*> badControlsList() const;
	/// Returns the list of flow switch controls that are good.
	QList<AMControl*> goodControlsList() const;

signals:
	/// Notifier that the controls have changed.
	void controlsChanged();

public slots:
	/// Adds a control to the group.
	bool addControl(AMControl *newControl);
	/// Adds a control to the group. Reimplemented to specify what good and bad values are.
	bool addControl(AMControl *newControl, double badValue, double goodValue);
	/// Removes a control from the group.
	bool removeControl(AMControl *control);
	/// Clears all controls from the group.
	bool clearControls();

protected:
	/// Creates and returns a move action. Always returns 0, as this control does not support moving.
	virtual AMAction3* createMoveAction(double setpoint) { Q_UNUSED(setpoint) return 0; }
	/// Returns the index for the current value.
	virtual int currentIndex() const;
};

#endif // BIOXASUTILITIESGROUP_H
