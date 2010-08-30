#ifndef AMBEAMLINECONTROLACTION_H
#define AMBEAMLINECONTROLACTION_H

#include "AMControl.h"
#include "AMBeamlineActionItem.h"

class AMBeamlineControlAction : public AMBeamlineActionItem
{
Q_OBJECT
public:
	AMBeamlineControlAction(AMControl *control, QObject *parent = 0);

	virtual QString type() const;

protected:
	AMControl *control_;

private:
	QString type_;
};



#endif // AMBEAMLINECONTROLACTION_H
