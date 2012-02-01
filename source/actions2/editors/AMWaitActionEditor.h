#ifndef AMWAITACTIONEDITOR_H
#define AMWAITACTIONEDITOR_H

#include <QFrame>
#include "actions2/actions/AMWaitActionInfo.h"

class QDoubleSpinBox;

/// This class implements a simple editor widget for AMWaitActionInfo that can be shown inside action tree views.  It provides a spin box to set the number of seconds to wait.
class AMWaitActionEditor : public QFrame
{
    Q_OBJECT
public:
    /// Constructor: Must be Q_INVOKABLE, and accept an AMWaitActionInfo as argument
    Q_INVOKABLE AMWaitActionEditor(AMWaitActionInfo* info, QWidget *parent = 0);

signals:

public slots:

protected slots:
    /// When editing is finished, calls info_->setSecondsToWait() with the value in the spin box.
    void onSpinBoxEditingFinished();

protected:
    QDoubleSpinBox* spinBox_;
    AMWaitActionInfo* info_;

};

#endif // AMWAITACTIONEDITOR_H