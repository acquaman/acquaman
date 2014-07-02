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


#include "AMDetectorTriggerSourceScanOptimizer.h"

#include "actions3/AMListAction3.h"
#include "actions3/actions/AMDetectorTriggerAction.h"
#include "beamline/AMDetectorTriggerSource.h"

 AMDetectorTriggerSourceScanOptimizer::~AMDetectorTriggerSourceScanOptimizer(){}
AMDetectorTriggerSourceScanOptimizer::AMDetectorTriggerSourceScanOptimizer(AMAction3 *scanActionTree, QObject *parent) :
	AMScanActionControllerScanOptimizer(scanActionTree, parent)
{
}

void AMDetectorTriggerSourceScanOptimizer::optimizeImplementation(AMAction3 *scanActionTree){
	QMap<AMDetectorTriggerSource*, AMListAction3*> triggerSourcesToActionLists;

	QList<AMAction3*> allDetectorAcquistionLists = AMScanActionTreeSupport::findActionsNamed(scanActionTree, "Acquire All Detectors");
	for(int x = 0; x < allDetectorAcquistionLists.count(); x++){
		triggerSourcesToActionLists.clear();

		AMListAction3 *castToListAction = qobject_cast<AMListAction3*>(allDetectorAcquistionLists.at(x));
		if(castToListAction){
			for(int y = 0; y < castToListAction->subActionCount(); y++){
				AMListAction3 *castSubToListAction = qobject_cast<AMListAction3*>(castToListAction->subActionAt(y));
				if(castSubToListAction && castSubToListAction->info()->shortDescription() == "Acquire One Detector"){
					AMDetectorTriggerAction *castToTriggerAction = qobject_cast<AMDetectorTriggerAction*>(castSubToListAction->subActionAt(0));
					if(castToTriggerAction && castToTriggerAction->detector()->detectorTriggerSource())
						triggerSourcesToActionLists.insertMulti(castToTriggerAction->detector()->detectorTriggerSource(), castSubToListAction);
				}
			}
		}
		QList<AMDetectorTriggerSource*> allSources = triggerSourcesToActionLists.uniqueKeys();
		for(int y = 0; y < allSources.count(); y++){
			QList<AMListAction3*> listsForSource = triggerSourcesToActionLists.values(allSources.at(y));
			if(listsForSource.count() > 1){
				AMListAction3 *combinedReadList = new AMListAction3(new AMListActionInfo3(QString("Read Detectors For %1").arg(allSources.at(y)->name()), QString("Read Detectors For %1").arg(allSources.at(y)->name())), AMListAction3::Parallel);
				AMListAction3 *firstAcquisitionList = listsForSource.at(0);
				combinedReadList->addSubAction(firstAcquisitionList->takeSubActionAt(1));
				for(int z = 1; z < listsForSource.count(); z++){
					AMListAction3 *castToAcquisitionList = qobject_cast<AMListAction3*>(listsForSource.at(z));
					if(castToAcquisitionList){
						combinedReadList->addSubAction(castToAcquisitionList->takeSubActionAt(1));
						castToAcquisitionList->deleteSubAction(0);
					}
				}
				firstAcquisitionList->addSubAction(combinedReadList);
				for(int z = 1; z < listsForSource.count(); z++){
					AMListAction3 *castParentToList = qobject_cast<AMListAction3*>(listsForSource.at(z)->parentAction());
					if(castParentToList)
						castParentToList->deleteSubAction(castParentToList->indexOfSubAction(listsForSource.at(z)));
				}
			}
		}
	}

}
