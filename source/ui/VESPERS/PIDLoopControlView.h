#ifndef PIDLOOPCONTROLVIEW_H
#define PIDLOOPCONTROLVIEW_H

#include <QWidget>

#include <QPropertyAnimation>
#include <QTimer>

#include "beamline/VESPERS/PIDLoopControl.h"

class PIDLoopControlView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.  Requires a valid PID loop control.
	explicit PIDLoopControlView(PIDLoopControl *pid, QWidget *parent = 0);
	/// Destructor.
	~PIDLoopControlView();

	/// Returns a pointer to the PID control being used by this view.
	PIDLoopControl *pid() const { return pid_; }

signals:

public slots:

protected slots:
	/// On visible changed.
	void onHiddenChanged(bool hidden);

protected:
	/// Custom interpolator for the QPalette.  Currently only changes the Button color.  \todo Make it a proper QPalette interpolator.
	QVariant paletteInterpolator(const QPalette &start, const QPalette &end, qreal progress);

	/// The pointer to the PID control.
	PIDLoopControl *pid_;

	/// The timer.
	QTimer *timer_;
	/// The animation.
	QPropertyAnimation *animation_;
};

#endif // PIDLOOPCONTROLVIEW_H
