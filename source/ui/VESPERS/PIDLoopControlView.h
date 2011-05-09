#ifndef PIDLOOPCONTROLVIEW_H
#define PIDLOOPCONTROLVIEW_H

#include <QWidget>

//#include <QPropertyAnimation>
#include <QTimer>
#include <QPushButton>

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
	/// Toggle the button color.
	void toggleButtonColor();

protected:
	/// The pointer to the PID control.
	PIDLoopControl *pid_;

	/// The timer.
	QTimer *timer_;
	/// Helper bool to know which color to put in the button.
	bool highlight_;
	/// The button.
	QPushButton *fix_;
};

#endif // PIDLOOPCONTROLVIEW_H
