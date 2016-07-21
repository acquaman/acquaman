#ifndef CLSBEAMSTATUSVALVES_H
#define CLSBEAMSTATUSVALVES_H

#include "beamline/CLS/CLSValves.h"

class AMListAction3;

#define CLSVALVES_BEAM_ONOFF_LIST_CONFLICTION 30010401

class CLSBeamStatusValves : public CLSValves
{
    Q_OBJECT
public:
	explicit CLSBeamStatusValves(const QString &name, QObject *parent = 0);
	~CLSBeamStatusValves();

public slots:
	/// Adds a valve control.
	/// @param beamOnOrder: the order to open a shutter when doing beam on/off. The smaller, the earlier. -1, if a shutter is not invovled in beam on/off
	bool addValve(AMControl *newValve, double openStateValue, double closedStateValue, int beamOnOrder=-1);
	/// Removes a valve control.
	bool removeValve(AMControl *newValve);
	/// Clears all valve controls.
	bool clearValves();

protected:
	/// Creates and returns a new move action to Open.
	AMAction3* createMoveToOpenAction();

protected:
	/// the order of the valves in the beam on action. If a valve is not invovled in the beam on/off action, the order is -1
	QMap<int, AMControl*> valvesBeamOnOrderMap_;
};

#endif // CLSBEAMSTATUSVALVES_H
