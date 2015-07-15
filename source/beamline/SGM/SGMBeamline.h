/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef AM_SGMBEAMLINE_H
#define AM_SGMBEAMLINE_H

#include "beamline/CLS/CLSBeamline.h"

class SGMMAXvMotor;
class AMMotorGroup;
class SGMBeamline : public CLSBeamline
{
Q_OBJECT
public:

	static SGMBeamline* sgm();		// singleton-class accessor
	virtual ~SGMBeamline();


	AMControl* energy() const { return energy_;}
	AMControl* exitSlitGap() const { return exitSlitGap_;}
	AMControl* grating() const { return grating_;}

	SGMMAXvMotor* ssaManipulatorX() const { return ssaManipulatorX_;}
	SGMMAXvMotor* ssaManipulatorY() const { return ssaManipulatorY_;}
	SGMMAXvMotor* ssaManipulatorZ() const { return ssaManipulatorZ_;}
	SGMMAXvMotor* ssaManipulatorRot() const { return ssaManipulatorRot_;}

	AMMotorGroup* ssaManipulatorMotorGroup() const { return ssaManipulatorMotorGroup_; }

	virtual CLSSIS3820Scaler* scaler() const { return scaler_; }
public slots:

signals:

protected slots:

protected:

	void setupBeamlineComponents();
	void setupMotorGroups();

	// Singleton implementation:
	SGMBeamline();					// protected constructor... only access through Beamline::bl()

	AMControl *energy_;
	AMControl *exitSlitGap_;
	AMControl *grating_;

	AMMotorGroup *ssaManipulatorMotorGroup_;

	SGMMAXvMotor *ssaManipulatorX_;
	SGMMAXvMotor *ssaManipulatorY_;
	SGMMAXvMotor *ssaManipulatorZ_;
	SGMMAXvMotor *ssaManipulatorRot_;

	CLSSIS3820Scaler *scaler_;

};


#endif // AM_SGMBEAMLINE_H
