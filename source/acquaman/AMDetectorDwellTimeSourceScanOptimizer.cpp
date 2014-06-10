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
	//QMap<AMDetectorDwellTimeSource*, AMListAction3*> dwellTimeSourcesToActionLists;
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
