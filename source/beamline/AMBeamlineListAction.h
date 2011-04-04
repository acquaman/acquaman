#ifndef AMBEAMLINELISTACTION_H
#define AMBEAMLINELISTACTION_H

#include "AMBeamlineParallelActionsList.h"

class AMBeamlineListAction : public AMBeamlineActionItem
{
Q_OBJECT
public:
	explicit AMBeamlineListAction(AMBeamlineParallelActionsList *list, QObject *parent = 0);

	virtual AMBeamlineActionView* createView(int index = 0);

	virtual AMBeamlineParallelActionsList* list();
	virtual void cleanup();

public slots:
	virtual void start();
	virtual void cancel();
	virtual void setList(AMBeamlineParallelActionsList *list);

protected slots:
	void delayedStart(bool ready);
	virtual void checkReady();
	virtual void initialize();
	virtual void calculateProgress(int stageIndex, double elapsed, double total);

protected:
	AMBeamlineParallelActionsList *list_;
};



#endif // AMBEAMLINELISTACTION_H
