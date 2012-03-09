#ifndef REIXSBEAMONOFFACTIONINFO_H
#define REIXSBEAMONOFFACTIONINFO_H


#include "actions2/AMActionInfo.h"

/// Specifies to REIXSBeamOnOffAction whether we should turn the beam on, or off.
class REIXSBeamOnOffActionInfo : public AMActionInfo
{
	Q_OBJECT
	Q_PROPERTY(bool beamOn READ beamOn WRITE setBeamOn)

public:
	/// Constructor. Specify whether to turn the beam on or off
	Q_INVOKABLE REIXSBeamOnOffActionInfo(bool beamOn = true, QObject *parent = 0);

	/// Copy Constructor
	REIXSBeamOnOffActionInfo(const REIXSBeamOnOffActionInfo& other) : AMActionInfo(other), beamOn_(other.beamOn_) {}

	/// This function is used as a virtual copy constructor
	virtual AMActionInfo* createCopy() const { return new REIXSBeamOnOffActionInfo(*this); }

	// Re-implemented public functions
	/////////////////////////////////

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "Beam On/Off"; }

	// New public functions
	//////////////////////////

	/// Returns true if the action will turn the beam on (otherwise turns it off)
	bool beamOn() const { return beamOn_; }

public slots:

	/// Set the total number of seconds to wait
	void setBeamOn(bool beamOn);

signals:


protected:
	/// True for beam ON
	bool beamOn_;

};

#endif // REIXSBEAMONOFFACTIONINFO_H
