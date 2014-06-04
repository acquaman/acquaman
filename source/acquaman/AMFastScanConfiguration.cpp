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


#include "AMFastScanConfiguration.h"

AMFastScanConfiguration::AMFastScanConfiguration(QObject *parent) :
    AMScanConfiguration(parent)
{
	startEnergy_ = -1;
	endEnergy_ = -1;
}

AMFastScanConfiguration::AMFastScanConfiguration(const AMFastScanConfiguration &original) :
	AMScanConfiguration(original)
{
	startEnergy_ = original.startEnergy();
	endEnergy_ = original.endEnergy();
}

AMFastScanConfiguration::~AMFastScanConfiguration(){}

const QMetaObject* AMFastScanConfiguration::getMetaObject(){
	return metaObject();
}

bool AMFastScanConfiguration::setStartEnergy(double startEnergy) {
	if(startEnergy != startEnergy_){
		startEnergy_ = startEnergy;
		setModified(true);
		emit energyRangeChanged();
	}
	return true;
}

bool AMFastScanConfiguration::setEndEnergy(double endEnergy) {
	if(endEnergy != endEnergy_){
		endEnergy_ = endEnergy;
		setModified(true);
		emit energyRangeChanged();
	}
	return true;
}
