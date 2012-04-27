/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").
Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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
