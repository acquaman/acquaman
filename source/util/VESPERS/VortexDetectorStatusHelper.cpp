/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "VortexDetectorStatusHelper.h"

VortexDetectorStatusHelper::VortexDetectorStatusHelper(QObject *parent) :
	QObject(parent)
{
	singleMCAto_ = new AMProcessVariable("IOC1607-004:mca1Read.SCAN", true, this);
	singleMCAfrom_ = new AMProcessVariable("IOC1607-004:mca1.SCAN", true, this);
	connect(singleMCAfrom_, SIGNAL(valueChanged(int)), singleMCAto_, SLOT(setValue(int)));
	//connect(singleMCAto_, SIGNAL(valueChanged(int)), this, SLOT(setSingleMCA(int)));

	singleDXPto_ = new AMProcessVariable("IOC1607-004:dxp1:ReadParams.SCAN", true, this);
	singleDXPfrom_ = new AMProcessVariable("IOC1607-004:dxp1.SCAN", true, this);
	connect(singleDXPfrom_, SIGNAL(valueChanged(int)), singleDXPto_, SLOT(setValue(int)));
	connect(singleDXPto_, SIGNAL(valueChanged(int)), this, SLOT(setSingleDXP(int)));

	fourMCAto_ = new AMProcessVariable("dxp1607-B21-04:ReadAll.SCAN", true, this);
	fourMCAfrom_ = new AMProcessVariable("dxp1607-B21-04:mca1.SCAN", true, this);
	connect(fourMCAfrom_, SIGNAL(valueChanged(int)), fourMCAto_, SLOT(setValue(int)));
	//connect(fourMCAto_, SIGNAL(valueChanged(int)), this, SLOT(setFourMCA(int)));

	fourDXPto_ = new AMProcessVariable("dxp1607-B21-04:ReadDXPs.SCAN", true, this);
	fourDXPfrom_ = new AMProcessVariable("dxp1607-B21-04:dxp1.SCAN", true, this);
	connect(fourDXPfrom_, SIGNAL(valueChanged(int)), fourDXPto_, SLOT(setValue(int)));
	connect(fourDXPto_, SIGNAL(valueChanged(int)), this, SLOT(setFourDXP(int)));

	fourEMaxTo_ = new AMProcessVariable("dxp1607-B21-04:mcaEMax", true, this);
	fourEMaxFrom_ = new AMProcessVariable("dxp1607-B21-04:dxp1.EMAX", true, this);
	connect(fourEMaxTo_, SIGNAL(valueChanged(double)), fourEMaxFrom_, SLOT(setValue(double)));
	connect(fourEMaxFrom_, SIGNAL(valueChanged(double)), this, SLOT(setFourEMax(double)));

	fourPeakTimeTo_ = new AMProcessVariable("dxp1607-B21-04:EnergyPkTime", true, this);
	fourPeakTimeFrom_ = new AMProcessVariable("dxp1607-B21-04:dxp1.PKTIM", true, this);
	connect(fourPeakTimeTo_, SIGNAL(valueChanged(double)), fourPeakTimeFrom_, SLOT(setValue(double)));
	connect(fourPeakTimeFrom_, SIGNAL(valueChanged(double)), this, SLOT(setFourPeakTime(double)));
}
