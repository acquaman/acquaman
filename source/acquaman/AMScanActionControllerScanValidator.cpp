#include "AMScanActionControllerScanValidator.h"

AMScanActionControllerScanValidator::AMScanActionControllerScanValidator(AMAction3 *scanActionTree, QObject *parent) :
	QObject(parent)
{
	scanActionTree_ = scanActionTree;
}

bool AMScanActionControllerScanValidator::validate(){
	return validateImplementation(scanActionTree_);
}
