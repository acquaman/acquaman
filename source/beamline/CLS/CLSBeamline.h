#ifndef CLSBEAMLINE_H
#define CLSBEAMLINE_H

#include "beamline/AMBeamline.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"


#define ERR_CLS_BEAM_ACTION_UNIMPLEMENTED 3001201
#define ERR_CLS_BEAM_ON_FAILED 3001202


#define CLSBEAMLINE_VALVE_OPEN 1
#define CLSBEAMLINE_VALVE_CLOSED 4

class AMAction3;

class CLSBeamline : public AMBeamline
{
    Q_OBJECT

public:
	/// Singleton accessor.
	static CLSBeamline *clsBeamline()
	{
		if (!instance_) {
			instance_ = new CLSBeamline("CLS Beamline");
			instance_->initializeBeamlineSupport();
		}

		return static_cast<CLSBeamline*>(instance_);
	}

	/// Destructor.
	virtual ~CLSBeamline();

	/// Returns the beamline's scaler.
	virtual CLSSIS3820Scaler* scaler() const { return 0; }

public slots:
	/// Handles turning beam on when button clicked
	void onTurningBeamOnRequested();
	/// Handles turning beam off when button clicked
	void onTurningBeamOffRequested();

protected slots:
	/// Handles disconnecting signals and deleting when beam on action is done
	void onBeamOnActionFinished();
	/// Handles disconnecting signals and deleting when beam on action is done
	void onBeamOnActionFailed();

	/// Handles disconnecting signals and deleting when beam off action is done
	void onBeamOffActionFinished();

protected:
	/// Protected constructor, for singleton pattern.
	CLSBeamline(const QString &controlName);

	/// Returns the list of actions to turn the beam on. Each beamline will have their own implementation.
	virtual AMAction3* createBeamOnActions() const;
	/// Returns the list of actions to turn the beam off.  Each beamline will have their own implementation.
	virtual AMAction3* createBeamOffActions() const;

protected:
	/// Our copy of the beam on action
	AMAction3 *beamOnAction_;
	/// Our copy of the beam off action
	AMAction3 *beamOffAction_;

};

#endif // CLSBEAMLINE_H
