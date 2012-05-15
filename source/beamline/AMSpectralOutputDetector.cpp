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


#include "AMSpectralOutputDetector.h"

AMSpectralOutputDetector::AMSpectralOutputDetector(const QString& name, AMControl *control, int numSpectrumBins, QString xElementName, QStringList yElementNames, QObject *parent) :
		AMSpectralOutputDetectorInfo(name, name, numSpectrumBins, xElementName, yElementNames, parent), AMDetector(name)
{
	control_ = control;
	connect(control_, SIGNAL(connected(bool)), AMDetector::signalSource(), SIGNAL(ready(bool)));
}

AMSpectralOutputDetector::~AMSpectralOutputDetector() {
	control_ = NULL;
}

const QMetaObject* AMSpectralOutputDetector::getMetaObject() {
	return metaObject();
}

AMDetectorInfo AMSpectralOutputDetector::toInfo(){
	return *AMDetectorInfo::toNewInfo();
}

bool AMSpectralOutputDetector::setControls(AMSpectralOutputDetectorInfo *detectorSettings){
	Q_UNUSED(detectorSettings)
	return false;
}

bool AMSpectralOutputDetector::setFromInfo(const AMDetectorInfo &info){
	Q_UNUSED(info)
	return false;
}
