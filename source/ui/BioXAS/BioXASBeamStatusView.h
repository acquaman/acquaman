#ifndef BIOXASBEAMSTATUSVIEW_H
#define BIOXASBEAMSTATUSVIEW_H

#include <QWidget>

#include "ui/CLS/CLSBeamlineStatusView.h"

class AMControl;
class CLSBeamlineStatus;

class BioXASBeamStatusView : public CLSBeamlineStatusView
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASBeamStatusView(CLSBeamlineStatus *beamStatus, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamStatusView();

protected:
	/// Creatse and returns a component view for the given control.
	virtual QWidget* createComponentView(AMControl *control);

};

#endif // BIOXASBEAMSTATUSVIEW_H
