#ifndef AMBEAMLINECONTROL_H
#define AMBEAMLINECONTROL_H

#include "beamline/AMControl.h"
#include "beamline/AMConnectedControl.h"

class AMBeamlineControl : public AMConnectedControl
{
	Q_OBJECT

public:
	/// Constructor.
	AMBeamlineControl(const QString &name, const QString &units, QObject *parent = 0);
	/// Destructor.
	virtual ~AMBeamlineControl();

	/// Returns true if this control can measure its value right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise. Subclasses should reimplement if they want to consider only a subset of children.
	virtual bool canMove() const { return false; }
	/// Returns true if this control can stop a move right now. False otherwise.
	virtual bool canStop() const { return false; }

	/// Returns true if the status is good.
	virtual bool isGood() const;
	/// Returns true if the status is bad.
	virtual bool isBad() const;

	/// Returns the current status.
	virtual double status() const;
	/// Returns the current value.
	virtual double value() const;

	/// Returns the status control.
	AMControl* statusControl() const { return statusControl_; }
	/// Returns the value control.
	AMControl* valueControl() const { return valueControl_; }

signals:
	/// Notifier that the status has changed.
	void statusChanged(double status);
	/// Notifier that the status control has changed.
	void statusControlChanged(AMControl *control);
	/// Notifier that the value control has changed.
	void valueControlChanged(AMControl *control);

public slots:
	/// Sets the status control.
	void setStatusControl(AMControl *control, double goodValue, double badValue);
	/// Sets the value control.
	void setValueControl(AMControl *control);

protected:
	/// The good status value.
	double goodStatusValue_;
	/// The bad status value.
	double badStatusValue_;
	/// The status control.
	AMControl *statusControl_;
	/// The value control.
	AMControl *valueControl_;
};

#endif // AMBEAMLINECONTROL_H
