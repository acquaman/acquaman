#ifndef AMLOOPACTIONEDITOR_H
#define AMLOOPACTIONEDITOR_H

#include <QFrame>
#include "actions2/AMLoopActionInfo.h"

class QSpinBox;

/// This class implements a simple editor widget for AMLoopActionInfo that can be shown inside action tree views.  It provides a spin box to set the number of iterations to run.
class AMLoopActionEditor : public QFrame
{
    Q_OBJECT
public:
    /// Constructor: Must be Q_INVOKABLE, and accept an AMLoopActionInfo as argument
    Q_INVOKABLE AMLoopActionEditor(AMLoopActionInfo* info, QWidget *parent = 0);

signals:

public slots:

protected slots:
    /// When editing is finished, calls info_->setLoopCount() with the value in the spin box.
    void onSpinBoxEditingFinished();

protected:
    QSpinBox* spinBox_;
    AMLoopActionInfo* info_;

};

#endif // AMLOOPACTIONEDITOR_H