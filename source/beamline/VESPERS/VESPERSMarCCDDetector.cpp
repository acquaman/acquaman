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


#include "VESPERSMarCCDDetector.h"

 VESPERSMarCCDDetector::~VESPERSMarCCDDetector(){}
VESPERSMarCCDDetector::VESPERSMarCCDDetector(const QString &name, const QString &description, QObject *parent)
	: VESPERSCCDDetector(name, description, parent)
{
	axes_ << AMAxisInfo("X-axis", 2048, "X dimension in pixels", "Counts")
		  << AMAxisInfo("Y-axis", 2048, "Y dimension in pixels", "Counts");

	acquireControl_ = new AMPVControl("Acquisition", "ccd1607-002:cam1:Acquire", "PAD1607-B21-05:cam1:Acquire", "PAD1607-B21-05:cam1:Acquire", this, 0.1, 10.0, 0);
	acquireTimeControl_ = new AMSinglePVControl("Acquire Time", "ccd1607-002:cam1:AcquireTime", this, 0.1);
	acquisitionStatusControl_ = new AMSinglePVControl("Detector Status", "ccd1607-002:cam1:Acquire", this, 0.1);
	ccdFilePathControl_ = new AMSinglePVControl("File Path", "ccd1607-002:cam1:FilePath", this);
	ccdFileBaseNameControl_ = new AMSinglePVControl("File Base Name", "ccd1607-002:cam1:FileName", this);
	ccdFileNumberControl_ = new AMSinglePVControl("File Number","ccd1607-002:cam1:FileNumber", this);

	allControlsCreated();
}

QString VESPERSMarCCDDetector::synchronizedDwellKey() const
{
	return "ccd1607-002:cam1:Acquire NPP NMS";
}
