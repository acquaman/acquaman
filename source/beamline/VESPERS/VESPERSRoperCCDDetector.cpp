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


#include "VESPERSRoperCCDDetector.h"
#include <QThreadPool>
#include <QMutexLocker>

 VESPERSRoperCCDDetector::~VESPERSRoperCCDDetector(){}
VESPERSRoperCCDDetector::VESPERSRoperCCDDetector(const QString &name, const QString &description, QObject *parent)
	: VESPERSCCDDetector(name, description, parent)
{
	axes_ << AMAxisInfo("X-axis", 2084, "X dimension in pixels", "Counts")
		  << AMAxisInfo("Y-axis", 2084, "Y dimension in pixels", "Counts");

	acquireControl_ = new AMPVControl("Acquisition", "IOC1607-003:det1:Acquire", "PAD1607-B21-05:cam1:Acquire", "PAD1607-B21-05:cam1:Acquire", this, 0.1, 10.0, 0);
	acquireTimeControl_ = new AMSinglePVControl("Acquire Time", "IOC1607-003:det1:AcquireTime", this, 0.1);
	acquisitionStatusControl_ = new AMSinglePVControl("Detector Status", "IOC1607-003:det1:Acquire", this, 0.1);
	ccdFilePathControl_ = new AMSinglePVControl("File Path", "IOC1607-003:det1:FilePath", this);
	ccdFileBaseNameControl_ = new AMSinglePVControl("File Base Name", "IOC1607-003:det1:FileName", this);
	ccdFileNumberControl_ = new AMSinglePVControl("File Number","IOC1607-003:det1:FileNumber", this);

	allControlsCreated();
}

QString VESPERSRoperCCDDetector::synchronizedDwellKey() const
{
	return "DIO1607-01:CCD:ExtSync NPP NMS";
}
