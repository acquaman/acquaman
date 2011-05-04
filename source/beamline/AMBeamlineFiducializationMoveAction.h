#ifndef AMBEAMLINEFIDUCIALIZATIONMOVEACTION_H
#define AMBEAMLINEFIDUCIALIZATIONMOVEACTION_H

#include "AMBeamlineControlSetMoveAction.h"

#define AMBEAMLINEACTIONITEM_INVALID_FIDUCIALIZATION_INDEX 27200

class AMBeamlineFiducializationMoveAction : public AMBeamlineControlSetMoveAction
{
Q_OBJECT
public:
    AMBeamlineFiducializationMoveAction(int fiducializationIndex, QObject *parent = 0);

    virtual AMBeamlineActionItemView* createView(int index = 0);

protected:
    int fiducializationIndex_;
};

class AMBeamlineFiducializationMoveActionView : public AMBeamlineControlSetMoveActionView
{
Q_OBJECT
public:
	AMBeamlineFiducializationMoveActionView(AMBeamlineFiducializationMoveAction *fiducializationAction, int index = 0, QWidget *parent = 0);

protected slots:
	void onInfoChanged();

protected:
	AMBeamlineFiducializationMoveAction *fiducializationAction_;
};

#endif // AMBEAMLINEFIDUCIALIZATIONMOVEACTION_H
