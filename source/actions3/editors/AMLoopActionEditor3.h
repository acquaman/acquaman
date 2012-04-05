#ifndef AMLOOPACTIONEDITOR3_H
#define AMLOOPACTIONEDITOR3_H

#include <QFrame>
#include "actions3/AMLoopActionInfo3.h"

class QSpinBox;

/// This class implements a simple editor widget for AMLoopActionInfo that can be shown inside action tree views.  It provides a spin box to set the number of iterations to run.
class AMLoopActionEditor3 : public QFrame
{
	Q_OBJECT
public:
	/// Constructor: Must be Q_INVOKABLE, and accept an AMLoopActionInfo as argument
	Q_INVOKABLE AMLoopActionEditor3(AMLoopActionInfo3* info, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// When editing is finished, calls info_->setLoopCount() with the value in the spin box.
	void onSpinBoxEditingFinished();

protected:
	QSpinBox* spinBox_;
	AMLoopActionInfo3* info_;

};

#endif // AMLOOPACTIONEDITOR_H
