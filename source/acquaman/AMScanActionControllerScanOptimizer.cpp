#include "AMScanActionControllerScanOptimizer.h"

#include "actions3/AMListAction3.h"

namespace AMScanOptimizerSupport{
	QList<AMAction3*> findActionsNamed(AMAction3 *rootAction, const QString &name){
		QList<AMAction3*> retVal;
		AMListAction3 *castToListAction = qobject_cast<AMListAction3*>(rootAction);
		if(castToListAction){
			for(int x = 0; x < castToListAction->subActionCount(); x++){
				AMListAction3 *subActionToListAction = qobject_cast<AMListAction3*>(castToListAction->subActionAt(x));
				if(subActionToListAction && subActionToListAction->info()->shortDescription() == name)
					retVal.append(subActionToListAction);
				retVal.append((findActionsNamed(castToListAction->subActionAt(x), name)));
			}
		}
		return retVal;
	}

	QList<AMAction3*> findActionsOfType(AMAction3 *rootAction, AMAction3 *typeAction){
		QList<AMAction3*> retVal;
		AMListAction3 *castToListAction = qobject_cast<AMListAction3*>(rootAction);
		if(castToListAction){
			for(int x = 0; x < castToListAction->subActionCount(); x++){
				if(typeAction->metaObject()->className() == castToListAction->subActionAt(x)->metaObject()->className())
					retVal.append(castToListAction->subActionAt(x));
				retVal.append((findActionsOfType(castToListAction->subActionAt(x), typeAction)));
			}
		}
		return retVal;
	}
}

AMScanActionControllerScanOptimizer::AMScanActionControllerScanOptimizer(AMAction3 *scanActionTree, QObject *parent) :
	QObject(parent)
{
	scanActionTree_ = scanActionTree;
}

AMAction3* AMScanActionControllerScanOptimizer::scanActionTree(){
	return scanActionTree_;
}

void AMScanActionControllerScanOptimizer::optimize(){
	optimizeImplementation(scanActionTree());
}
