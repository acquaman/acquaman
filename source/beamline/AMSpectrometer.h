/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#ifndef ACQMAN_SPECTROMETER_H_
#define ACQMAN_SPECTROMETER_H_

#include "AMControl.h"

class AMHexapod : public AMControl {
	Q_OBJECT
public:
		AMHexapod(const QString& name, QObject* parent = 0) : AMControl(name, "", parent) {
			x_ = new AMPVwStatusControl("x", "HXPD1610-4-I20:X:sp", "HXPD1610-4-I20:X", "HXPD1610-4-I20:moving", "HXPD1610-4-I20:stop", this, 0.01);

		// x_ = new PVControl("x", "SMTR1610-4-I20-RES-X:step:sp", "SMTR1610-4-I20-RES-X:step", "SMTR1610-4-I20-RES-X:moving", 0.01, this);
		// x_ = new PVControl("x", "TESTPV1610-4-I20:ai", "TESTPV1610-4-I20:ao", "", 0.01, this);

		children_ << x_;
				y_ = new AMReadOnlyPVwStatusControl("y", "HXPD1610-4-I20:Y:sp", "HXPD1610-4-I20:moving", this);
		children_ << y_;
				z_ = new AMReadOnlyPVwStatusControl("z", "HXPD1610-4-I20:Z:sp", "HXPD1610-4-I20:moving", this);
		children_ << z_;
	}

		AMControl* x() const		{ return x_; }
		AMControl* y() const		{ return y_; }
		AMControl* z() const		{ return z_; }

		AMControl *y_, *z_;
		AMControl* x_;
};

class AMSpectrometer : public AMControl
{
	Q_OBJECT
public:
		AMSpectrometer(const QString& name, QObject* parent = 0) : AMControl(name, "", parent) {

				hexapod_ = new AMHexapod("hexapod", this);
		children_ << hexapod_;

	}


		AMHexapod* hexapod() const		{ return hexapod_; }

		virtual ~AMSpectrometer() {}

protected:
		AMHexapod* hexapod_;

};

#endif /*ACQMAN_SPECTROMETER_H_*/
