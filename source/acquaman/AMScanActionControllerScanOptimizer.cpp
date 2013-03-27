#include "AMScanActionControllerScanOptimizer.h"

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
