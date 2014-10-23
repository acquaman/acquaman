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


#include "AMDetectorDwellTimeSourceScanOptimizer.h"

#include "actions3/AMListAction3.h"
#include "actions3/actions/AMDetectorDwellTimeAction.h"
#include "beamline/AMDetectorTriggerSource.h"

 AMDetectorDwellTimeSourceScanOptimizer::~AMDetectorDwellTimeSourceScanOptimizer(){}
AMDetectorDwellTimeSourceScanOptimizer::AMDetectorDwellTimeSourceScanOptimizer(AMAction3 *scanActionTree, QObject *parent) :
	AMScanActionControllerScanOptimizer(scanActionTree, parent)
{
}

void AMDetectorDwellTimeSourceScanOptimizer::optimizeImplementation(AMAction3 *scanActionTree){
	QList<AMDetectorDwellTimeSource*> dwellTimeSources;
	QList<AMAction3*> tagForRemoval;

	QList<AMAction3*> allDetectorDwellSetLists = AMScanActionTreeSupport::findActionsNamed(scanActionTree, "Set All Detectors Dwell Times");
	for(int x = 0; x < allDetectorDwellSetLists.count(); x++){
		dwellTimeSources.clear();
		tagForRemoval.clear();

		AMListAction3 *castToListAction = qobject_cast<AMListAction3*>(allDetectorDwellSetLists.at(x));
		if(castToListAction){
			for(int y = 0; y < castToListAction->subActionCount(); y++){
				AMDetectorDwellTimeAction *castToDwellTimeAction = qobject_cast<AMDetectorDwellTimeAction*>(castToListAction->subActionAt(y));
				if(castToDwellTimeAction && castToDwellTimeAction->detector()->detectorDwellTimeSource() && !dwellTimeSources.contains(castToDwellTimeAction->detector()->detectorDwellTimeSource()))
					dwellTimeSources.append(castToDwellTimeAction->detector()->detectorDwellTimeSource());
				else if(castToDwellTimeAction && castToDwellTimeAction->detector()->detectorDwellTimeSource())
					tagForRemoval.append(castToDwellTimeAction);
			}

			for(int y = 0; y < tagForRemoval.count(); y++)
				castToListAction->deleteSubAction(castToListAction->indexOfSubAction(tagForRemoval.at(y)));
		}
	}

}
