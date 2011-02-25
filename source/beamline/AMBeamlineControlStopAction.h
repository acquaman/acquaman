#ifndef AMBEAMLINECONTROLSTOPACTION_H
#define AMBEAMLINECONTROLSTOPACTION_H

#include "AMBeamlineActionItem.h"
#include "AMControl.h"

class AMBeamlineControlStopAction : public AMBeamlineActionItem
{
Q_OBJECT
public:
    AMBeamlineControlStopAction(AMControl *control, QObject *parent = 0);

    virtual QString type() const;
    virtual AMControl* control();

public slots:
    virtual void start();
    virtual void setControl(AMControl *control);
    virtual void cleanup(){}

protected slots:
    virtual void onMovingChanged(bool moving);
    virtual void onStopped();
    virtual void onFailed(int explanation);

protected:
	AMControl *control_;

private:
	QString type_;
};

#endif // AMBEAMLINECONTROLSTOPACTION_H
