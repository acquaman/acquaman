#include "AMListActionScanOptimizer.h"

#include "actions3/AMListAction3.h"

 AMEmptyListScanOptimizer::~AMEmptyListScanOptimizer(){}
AMEmptyListScanOptimizer::AMEmptyListScanOptimizer(AMAction3 *scanActionTree, QObject *parent) :
	AMScanActionControllerScanOptimizer(scanActionTree, parent)
{
}

void AMEmptyListScanOptimizer::optimizeImplementation(AMAction3 *scanActionTree){
	AMListAction3 *templateListAction = new AMListAction3(new AMListActionInfo3("Fake List", "Fake List"));
	QList<AMAction3*> allListsAsActions = AMScanActionTreeSupport::findActionsOfType(scanActionTree, templateListAction);

	QList<AMListAction3*> allLists;
	AMListAction3 *tempListAction;
	for(int x = 0; x < allListsAsActions.count(); x++){
		tempListAction = qobject_cast<AMListAction3*>(allListsAsActions.at(x));
		if(tempListAction)
			allLists.append(tempListAction);
	}

	for(int x = 0; x < allLists.count(); x++){
		if(allLists.at(x)->subActionCount() == 0){
			AMListAction3 *castParentToList = qobject_cast<AMListAction3*>(allLists.at(x)->parentAction());
			if(castParentToList)
				castParentToList->deleteSubAction(castParentToList->indexOfSubAction(allLists.at(x)));
		}
	}

	delete templateListAction;
}

 AMSingleElementListOptimizer::~AMSingleElementListOptimizer(){}
AMSingleElementListOptimizer::AMSingleElementListOptimizer(AMAction3 *scanActionTree, QObject *parent) :
	AMScanActionControllerScanOptimizer(scanActionTree, parent)
{
}

void AMSingleElementListOptimizer::optimizeImplementation(AMAction3 *scanActionTree){
	AMListAction3 *templateListAction = new AMListAction3(new AMListActionInfo3("Fake List", "Fake List"));
	QList<AMAction3*> allListsAsActions = AMScanActionTreeSupport::findActionsOfType(scanActionTree, templateListAction);

	QList<AMListAction3*> allLists;
	AMListAction3 *tempListAction;
	for(int x = 0; x < allListsAsActions.count(); x++){
		tempListAction = qobject_cast<AMListAction3*>(allListsAsActions.at(x));
		if(tempListAction)
			allLists.append(tempListAction);
	}

	for(int x = 0; x < allLists.count(); x++){
		if(allLists.at(x)->subActionCount() == 1){
			AMListAction3 *castParentToList = qobject_cast<AMListAction3*>(allLists.at(x)->parentAction());
			if(castParentToList){
				int indexOfThisList = castParentToList->indexOfSubAction(allLists.at(x));
				castParentToList->insertSubAction(allLists.at(x)->takeSubActionAt(0), indexOfThisList);
				castParentToList->deleteSubAction(castParentToList->indexOfSubAction(allLists.at(x)));
			}
		}
	}
}
