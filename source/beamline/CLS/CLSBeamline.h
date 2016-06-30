#ifndef CLSBEAMLINE_H
#define CLSBEAMLINE_H

#include "beamline/AMBeamline.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"


#define CLSBEAMLINE_ERR_BEAM_ON_FAILED 3001201
#define CLSBEAMLINE_ERR_BEAM_ON_ALREADY_ON 3001202
#define CLSBEAMLINE_ERR_BEAM_ON_UNCONNECTED_PV 3001203
#define CLSBEAMLINE_ERR_BEAM_ON_CLOSED_SAFETY_SHUTTER 3001204
#define CLSBEAMLINE_ERR_BEAM_OFF_UNCONNECTED_PV 3001210
#define CLSBEAMLINE_ERR_BEAM_OFF_ALREADY_OFF 3001211

#define CLSBEAMLINE_VALVE_OPEN 1
#define CLSBEAMLINE_VALVE_CLOSED 4

class AMAction3;
class CLSBeamlineStatus;
class CLSShutters;
class CLSValves;

class CLSBeamline : public AMBeamline
{
    Q_OBJECT

public:
	/// Singleton accessor.
	static CLSBeamline *clsBeamline()
	{
		if (!instance_) {
			instance_ = new CLSBeamline("CLSBeamline", "CLS Beamline");
			instance_->initializeBeamlineSupport();
		}

		return static_cast<CLSBeamline*>(instance_);
	}

	/// Destructor.
	virtual ~CLSBeamline();

	/// Retuns whether the beamline components are connected or not
	virtual bool isConnected() const;

	/// Returns the beamline status component
	CLSBeamlineStatus* beamlineStatus() const;
	/// Returns the beamline's scaler.
	virtual CLSSIS3820Scaler* scaler() const { return 0; }

signals:
	/// signal to indicate beam availability changed
	void beamAvaliabilityChanged(bool beamOn);

public slots:
	/// Handles turning beam on when button clicked
	void onTurningBeamOnRequested();
	/// Handles turning beam off when button clicked
	void onTurningBeamOffRequested();

protected slots:
	/// Slot to check the connected status of the beamline components
	void onBeamlineComponentConnected();

	/// Updates the beam status changed
	void updateBeamStatus();

	/// Handles disconnecting signals and deleting when beam on action is done
	void onBeamOnActionFinished();
	/// Handles disconnecting signals and deleting when beam on action is done
	void onBeamOnActionFailed();

	/// Handles disconnecting signals and deleting when beam off action is done
	void onBeamOffActionFinished();

protected:
	/// Protected constructor, for singleton pattern.
	CLSBeamline(const QString &beamlineName, const QString &controlName);

	/// set the beamline name
	void setBeamlineName(const QString &name) { beamlineName_ = name; }

	/// helper funtion to create beamline status and beamline shutters and beamline valves
	virtual void createBeamlineStatus();

	/// helper function to setup the beamline status component
	void setBeamlineStatus(CLSBeamlineStatus *beamlineStatus);

	/// Returns the list of actions to turn the beam on. Each beamline will have their own implementation.
	virtual AMListAction3* createBeamOnActions() const;
	/// Returns the list of actions to turn the beam off.  Each beamline will have their own implementation.
	virtual AMListAction3* createBeamOffActions() const;

protected:
	/// The name of the beamline
	QString beamlineName_;

	/// The latest connectivity status
	bool wasConnected_;
	/// The latest beam status
	bool wasBeamOn_;

	/// The beamline status component
	CLSBeamlineStatus *beamlineStatus_;
	/// The shutters control.
	CLSShutters *beamlineShutters_;
	/// The valves control.
	CLSValves *beamlineValves_;

	/// Our copy of the beam on action
	AMAction3 *beamOnAction_;
	/// Our copy of the beam off action
	AMAction3 *beamOffAction_;

};

#endif // CLSBEAMLINE_H
