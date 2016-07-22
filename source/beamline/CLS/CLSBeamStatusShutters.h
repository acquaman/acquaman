#ifndef CLSBEAMSTATUSSHUTTERS_H
#define CLSBEAMSTATUSSHUTTERS_H

#include "beamline/CLS/CLSShutters.h"

class AMListAction3;

#define CLSBEAMSTATUSSHUTTERS_BEAM_ONOFF_LIST_CONFLICTION 30010301

class CLSBeamStatusShutters : public CLSShutters
{
    Q_OBJECT
public:
	explicit CLSBeamStatusShutters(const QString &name, QObject *parent = 0);
	~CLSBeamStatusShutters();

	/// Returns whether it is safe to open the shutters, default True
	virtual bool isSafeToOpenShutters() const;

	/// Returns the safety shutter control
	AMControl *safetyShutter() const { return safetyShutter_;}

signals:
	/// Notifier that the safety shutter control has changed.
	void safetyShutterChanged(AMControl *newControl);

public slots:
	/// Adds a shutter control.
	/// @param beamOnOrder: the order to open a shutter when doing beam on/off. The smaller, the earlier. -1, if a shutter is not invovled in beam on/off
	bool addShutter(AMControl *newShutter, double openStateValue, double closedStateValue, int beamOnOrder=-1);
	/// Removes a shutter control.
	bool removeShutter(AMControl *newValve);
	/// Clears all shutter controls.
	bool clearShutters();

	/// set the safety shutter
	void setSafetyShutter(AMControl *safetyShutter, double openStateValue, double closedStateValue, int beamOnOrder=-1);

protected:
	/// Creates and returns a new move action to Open.
	virtual AMAction3* createMoveToOpenAction();
	/// Creates and returns a new move action to Closed.
	virtual AMAction3* createMoveToClosedAction();

protected:
	/// the safety shutter of a beamline
	AMControl *safetyShutter_;

	/// the order of the shutters in the beam on action. If a shutter is not invovled in the beam on/off action, the order is -1
	QMap<int, AMControl*> shuttersBeamOnOrderMap_;
};

#endif // CLSBEAMSTATUSSHUTTERS_H
