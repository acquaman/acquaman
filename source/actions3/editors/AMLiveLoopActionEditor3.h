#ifndef AMLIVELOOPACTIONEDITOR3_H
#define AMLIVELOOPACTIONEDITOR3_H

#include <QFrame>
#include "actions3/AMLoopAction3.h"

class QSpinBox;
class QLabel;

/// This class implements an editor of a live AMLoopAction.  It is different from the normal editor because it modifies the action as it's running.  This is distinct difference in paradigm.
class AMLiveLoopActionEditor3 : public QFrame
{
	Q_OBJECT

public:
	/// Constructor.  Accepts an AMLoopAction (NOT info!).  If modifying actions becomes more necessary, this will need to be made Q_INVOKABLE.
 	virtual ~AMLiveLoopActionEditor3();
	AMLiveLoopActionEditor3(AMLoopAction3 *action, QWidget *parent = 0);

protected slots:
	/// When the action loop count updates.  Update the spin box accordingly.
	void onLoopCountChanged();
	/// Sets the new loop count.
	void setLoopCount();
	/// Updates the current iteration label when the iteration changes.
	void onIterationUpdate(int val);

protected:
	/// The action pointer.
	AMLoopAction3 *action_;
	/// The spin box holding the maximum loop count.
	QSpinBox *loopCount_;
	/// The label holding the current iteration of the loop.
	QLabel *iteration_;
};

#endif // AMLIVELOOPACTIONEDITOR3_H
