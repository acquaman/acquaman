#ifndef AMWAITACTIONEDITOR_H
#define AMWAITACTIONEDITOR_H

#include <QFrame>

#include "actions3/actions/AMWaitActionInfo.h"

#include <QDoubleSpinBox>

/// Simple editor that lets you change the wait time.
class AMWaitActionEditor : public QFrame
{
	Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE AMWaitActionEditor(AMWaitActionInfo *info, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMWaitActionEditor() {}

protected slots:
	/// Updates the spin box.
	void onWaitTimeChanged(double newTime);
	/// Sets the info with a new time.
	void setWaitTime();

protected:
	/// Holds onto the info.
	AMWaitActionInfo *info_;
	/// The spin box that updates the time.
	QDoubleSpinBox *time_;
};

#endif // AMWAITACTIONEDITOR_H
