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

#ifndef VESPERSBEAMLINE_H
#define VESPERSBEAMLINE_H

#include "beamline/AMBeamline.h"
#include "beamline/AMControlSet.h"

/// This class
class VESPERSBeamline : public AMBeamline
{
	Q_OBJECT
public:
	static VESPERSBeamline* vespers() {
		if(!instance_)
			instance_ = new VESPERSBeamline();
		return static_cast<VESPERSBeamline*>(instance_);
	}

	// Accessing control elements:

	AMControl *ccdDetector() const { return ccdDetector_; }

	// These Control Sets are logical groups of controls, that are commonly used by different Acquaman components

signals:

public slots:

protected:
	/// Constructor. This is a singleton class; access it through VESPERSBeamline::vespers().
	VESPERSBeamline();

	AMControl *ccdDetector_;

};

#endif // VESPERSBEAMLINE_H
