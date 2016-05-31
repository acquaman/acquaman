#ifndef CLSCONTROLSSTATUS_H
#define CLSCONTROLSSTATUS_H

#include "beamline/AMControlSet.h"
#include "beamline/CLS/CLSBiStateGroup.h"

class CLSControlsStatus : CLSBiStateGroup
{
    Q_OBJECT

public:

    /// Enumeration of the possible value states.
    enum Value { On = 0, Off = 1};

    /// Constructor.
    explicit CLSControlsStatus(const QString &name, QObject *parent = 0);

    /// Destructor.
    virtual ~CLSControlsStatus();

    /// Returns true if this control can move, false otherwise. Reimplemented to make all status controls read-only by default (for now).
    virtual bool canMove() const { return false; }

    /// Returns true if the control is on, false otherwise.
    virtual bool isOn() const;

    /// Returns true if the control is off, false otehrwise.
    virtual bool isOff() const;

    virtual bool componentInOnState(AMControl *control) const { return isChildState1(control); }

    virtual bool componentNotInOnState(AMControl *control) const { return !componentInOnState(control); }

    /// Returns true if the given component is a shutter control
    bool isShutterControl(AMControl * control) const { return shuttersControlSet_->indexOf(control) >= 0; }
    /// Returns true if the given component is a valve control
    bool isValveControl(AMControl * control) const { return valvesControlSet_->indexOf(control) >= 0; }
    /// Returns true if the given component is a mirror mask control
    bool isMirrorMaskControl(AMControl * control) const { return mirrorMaskControlSet_->indexOf(control) >= 0; }
    /// Returns true if the given component is a mono mask control
    bool isMonoMaskControl(AMControl * control) const { return monoMaskControlSet_->indexOf(control) >= 0; }

    /// Returns the beam on value for the given control.
    double componentBeamOnValue(AMControl *control) const { return controlState1ValueMap_.value(control, -1); }


    /// Returns the list of components.
    QList<AMControl*> components() const { return children_; }
    /// Returns the list of components that are in the 'beam on' state.
    QList<AMControl*> componentsInOnState() const;
    /// Returns the list of components that are not in the 'beam on' state.
    QList<AMControl*> componentsNotInOnState() const;

signals:
    /// Notifier that the components have changed.
    void componentsChanged();

public slots:

    /// Adds a shutter control. Returns true if successful, false otherwise.
    bool addShutterControl(AMControl *newControl, double onValue);
    /// Adds a valves control. Returns true if successful, false otherwise.
    bool addValveControl(AMControl *newControl, double onValue);
    /// Adds a mirror mask control. Returns true if successful, false otherwise.
    bool addMirrorMaskControl(AMControl *newControl, double onValue);
    /// Adds a mono mask control. Returns true if successful, false otherwise.
    bool addMonoMaskControl(AMControl *newControl, double onValue);

    /// Removes a component from the beam status. Returns true if successful, false otherwise.
    bool removeComponent(AMControl *control);
    /// Clears all components from the beam status. Returns true if successful, false otherwise.
    bool clearComponents();

protected:

    /// Adds a component to the beam status. Returns true if successful, false otherwise.
    bool addComponent(AMControl *newControl, double onValue);

    /// Creates and returns a new move action.
    virtual AMAction3* createMoveAction(double setpoint) { Q_UNUSED(setpoint) return 0; }

    /// Returns the index for the current value.
    virtual int currentIndex() const;


    /// the control set of the beamline shutters
    AMControlSet *shuttersControlSet_;

    /// the control set of the beamline valves
    AMControlSet *valvesControlSet_;

    /// the control set of the beamline mirror mask
    AMControlSet *mirrorMaskControlSet_;

    /// the control set of the beamline mono mask
    AMControlSet *monoMaskControlSet_;


};

#endif // CLSCONTROLSSTATUS_H
