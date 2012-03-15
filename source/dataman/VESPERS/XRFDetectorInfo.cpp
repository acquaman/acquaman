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


#include "XRFDetectorInfo.h"

XRFDetectorInfo::XRFDetectorInfo(const QString &name, const QString &description, QObject *parent)
	: AMBasicXRFDetectorInfo(name, description, parent)
{
	setUnits("Counts");
}

XRFDetectorInfo::XRFDetectorInfo(const XRFDetectorInfo &original)
	: AMBasicXRFDetectorInfo(original)
{
//	retreiveAndSetProperties(original);
	this->operator =(original);
}

XRFDetectorInfo &XRFDetectorInfo::operator =(const XRFDetectorInfo &other)
{
	if (this != &other){
		//retreiveAndSetProperties(other);
		AMBasicXRFDetectorInfo::operator =(other);
		setElements(other.elements());
		setChannels(other.channels());
		setIntegrationTime(other.integrationTime());
		setPeakingTime(other.peakingTime());
		AMROIInfoList *roiInfoList = const_cast<AMROIInfoList *>(other.roiInfoList());
		setROIList(*roiInfoList);
		setMaximumEnergy(other.maximumEnergy());
	}
	return *this;
}

void XRFDetectorInfo::setMaximumEnergy(double energy)
{
	maxEnergy_ = energy;
	double newScale = scale();

	for (int i = 0; i < roiInfoList()->count(); i++)
		(*roiInfoList())[i].setScale(newScale);

	setModified(true);
}
