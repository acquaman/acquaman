#ifndef SGMADDONSCOORDINATOR_H
#define SGMADDONSCOORDINATOR_H

#include "beamline/AMControlSet.h"

class SGMAddOnsCoordinator : public QObject
{
Q_OBJECT
public:
	SGMAddOnsCoordinator(QObject *parent = 0);

protected slots:
	void onSGMGratingControlChanged(double sgmGrating);
	void onAddOnsGratingControlChanged(double addOnsGrating);
	void onAllControlsConnected(bool connected);

protected:
	AMControl *SGMGratingControl_;
	AMControl *AddOnsGratingControl_;

	AMControlSet *allControls_;
	bool connectedOnce_;
};

#endif // SGMADDONSCOORDINATOR_H
