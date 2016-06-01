#include "CLSControlsStatusButtonBar.h"

#include "beamline/CLS/CLSControlsStatus.h"

CLSControlsStatusButtonBar::CLSControlsStatusButtonBar(CLSControlsStatus *controlsStatus, QWidget *parent) :
    CLSControlButtonBar(parent)
{

    controlsStatus_ = 0;

    setControlsStatus(controlsStatus);

}

CLSControlsStatusButtonBar::~CLSControlsStatusButtonBar()
{

}

void CLSControlsStatusButtonBar::refresh()
{
    clearControls();

    if(controlsStatus_){
        foreach(AMControl *control, controlsStatus_->components())
            addControl(control, controlsStatus_->componentOnValue(control));
    }
}

void CLSControlsStatusButtonBar::setControlsStatus(CLSControlsStatus *newControl)
{
    if(controlsStatus_ != newControl){
        if (controlsStatus_)
            disconnect(controlsStatus_, 0, this, 0);

        controlsStatus_ = newControl;

        if(controlsStatus_)
            connect(controlsStatus_, SIGNAL(componentsChanged()), this, SLOT(refresh()));

        refresh();

        emit controlStatusChanged(controlsStatus_);
    }
}

QAbstractButton* CLSControlsStatusButtonBar::createButton(AMControl *control, double greenValue)
{
    QAbstractButton *button = CLSControlButtonBar::createButton(control, greenValue);

    if (button) {
            QIcon buttonIcon;
            QString toolTip = "";
            if (controlsStatus_->isShutterControl(control)) {
                    buttonIcon = QIcon(":/shutterIcon2.png");
                    toolTip = "Shutters";
            } else if (controlsStatus_->isValveControl(control)) {
                    buttonIcon = QIcon(":/valveIcon2.png");
                    toolTip = "Valves";
            } else if (controlsStatus_->isMirrorMaskControl(control)) {
                    buttonIcon = QIcon(":/mirror-icon1.png");
                    toolTip = "Mirror Mask";
            } else if (controlsStatus_->isMonoMaskControl(control)) {
                    buttonIcon =QIcon(":/mono-icon5.png");
                    toolTip = "Monochromator Mask";
            }

            if (toolTip.length() > 0) {
                    button->setIcon(buttonIcon);
                    button->setToolTip(toolTip);
            }
    }

    return button;


}
