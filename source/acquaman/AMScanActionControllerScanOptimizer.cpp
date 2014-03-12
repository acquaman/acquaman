#include "AMScanActionControllerScanOptimizer.h"

#include "actions3/AMListAction3.h"

 AMScanActionControllerScanOptimizer::~AMScanActionControllerScanOptimizer(){}
AMScanActionControllerScanOptimizer::AMScanActionControllerScanOptimizer(AMAction3 *scanActionTree, QObject *parent) :
	QObject(parent)
{
	scanActionTree_ = scanActionTree;
}

void AMScanActionControllerScanOptimizer::optimize(){
	if(!scanActionTree_)
		return;
	optimizeImplementation(scanActionTree_);
}

void AMScanActionControllerScanOptimizer::setScanActionTree(AMAction3 *scanActionTree){
	scanActionTree_ = scanActionTree;
}
