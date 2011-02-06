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

#include "REIXSBeamline.h"

REIXSBeamline::REIXSBeamline(QObject *parent) :
	AMBeamline("REIXSBeamline")
{
}








//#include "AMBeamline.h"

//AMBeamline* AMBeamline::instance_ = 0;


//AMBeamline::AMBeamline() : AMControl("beamline", "n/a") {

//	ringCurrent_ = new AMReadOnlyPVControl("ringCurrent", AMPVNames::toPV("ringCurrent"), this);
//        children_ << ringCurrent_;

//		spectrometer_ = new AMSpectrometer("spectrometer", this);
//		children_ << spectrometer_;

//}

//AMBeamline::~AMBeamline()
//{
//}

//AMBeamline* AMBeamline::bl() {

//	if(instance_ == 0)
//                instance_ = new AMBeamline();

//	return instance_;

//}

//void AMBeamline::releaseBl() {

//	if(instance_) {
//		delete instance_;
//		instance_ = 0;
//	}

//}


