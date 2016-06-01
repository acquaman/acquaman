#ifndef CLSCONTROLSSTATUSBUTTONBAR_H
#define CLSCONTROLSSTATUSBUTTONBAR_H

#include "ui/CLS/CLSControlButtonBar.h"

class CLSControlsStatus;

class CLSControlsStatusButtonBar : public CLSControlButtonBar
{
        Q_OBJECT

public:
    CLSControlsStatusButtonBar(CLSControlsStatus *controlsStatus, QWidget *parent = 0);

    virtual ~CLSControlsStatusButtonBar();

    CLSControlsStatus* controlsStatus() const { return controlsStatus_; }


signals:
    void controlStatusChanged(CLSControlsStatus *newControl);

public slots:
    void refresh();

    void setControlsStatus(CLSControlsStatus *newControl);

protected:

    virtual QAbstractButton* createButton(AMControl *control, double greenValue);


protected:
    CLSControlsStatus *controlsStatus_;
};

#endif // CLSCONTROLSSTATUSBUTTONBAR_H
