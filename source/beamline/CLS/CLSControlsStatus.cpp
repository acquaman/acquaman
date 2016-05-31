#include "CLSControlsStatus.h"

CLSControlsStatus::CLSControlsStatus(const QString &name, QObject *parent) :
    CLSBiStateGroup(name, parent)
{
    shuttersControlSet_ = new AMControlSet(this);
    valvesControlSet_ = new AMControlSet(this);
    mirrorMaskControlSet_ = new AMControlSet(this);
    monoMaskControlSet_ = new AMControlSet(this);

    addOption(On, "On", true);
    addOption(Off, "Off", true);

}

CLSControlsStatus::~CLSControlsStatus()
{
    shuttersControlSet_->clear();
    valvesControlSet_->clear();
    mirrorMaskControlSet_->clear();
    monoMaskControlSet_->clear();

}

bool CLSControlsStatus::isOn() const
{
    bool result = false;

    if (isConnected() && areAllChildrenState1())
            result = true;

    return result;
}

bool CLSControlsStatus::isOff() const
{
    return !isOn();
}

QList<AMControl*> CLSControlsStatus::componentsInOnState() const
{
    return childrenInState1();
}

QList<AMControl*> CLSControlsStatus::componentsNotInOnState() const
{
    return childrenNotInState1();
}

bool CLSControlsStatus::addShutterControl(AMControl *newControl, double onValue)
{
    bool result = addComponent(newControl, onValue);
    if(result)
        shuttersControlSet_->addControl(newControl);

    return result;
}

bool CLSControlsStatus::addValveControl(AMControl *newControl, double onValue)
{
        bool result = addComponent(newControl, onValue);
        if (result)
                valvesControlSet_->addControl(newControl);

        return result;
}

bool CLSControlsStatus::addMirrorMaskControl(AMControl *newControl, double onValue)
{
        bool result = addComponent(newControl, onValue);
        if (result)
                mirrorMaskControlSet_->addControl(newControl);

        return result;
}

bool CLSControlsStatus::addMonoMaskControl(AMControl *newControl, double onValue)
{
        bool result = addComponent(newControl, onValue);
        if (result)
                monoMaskControlSet_->addControl(newControl);

        return result;
}

bool CLSControlsStatus::removeComponent(AMControl *control)
{
        bool result = removeBiStateControl(control);

        if (result) {
                if (isShutterControl(control))
                        shuttersControlSet_->removeControl(control);
                else if (isValveControl(control))
                        valvesControlSet_->removeControl(control);
                else if (isMirrorMaskControl(control))
                        mirrorMaskControlSet_->removeControl(control);
                else if (isMonoMaskControl(control))
                        monoMaskControlSet_->removeControl(control);

                emit componentsChanged();
        }

        return result;
}

bool CLSControlsStatus::clearComponents()
{
        bool result = clearBiStateControls();

        if (result)
                emit componentsChanged();

        return result;
}

bool CLSControlsStatus::addComponent(AMControl *newControl, double onValue)
{
        bool result = addBiStateControl(newControl, onValue);

        if (result)
                emit componentsChanged();

        return result;
}

int CLSControlsStatus::currentIndex() const
{
        int result = enumNames().indexOf("Unknown");

        if (isOn())
                result = On;
        else if (isOff())
                result = Off;

        return result;
}
