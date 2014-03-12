#include "AMScanActionControllerScanValidator.h"

 AMScanActionControllerScanValidator::~AMScanActionControllerScanValidator(){}
AMScanActionControllerScanValidator::AMScanActionControllerScanValidator(AMAction3 *scanActionTree, QObject *parent) :
	QObject(parent)
{
	scanActionTree_ = scanActionTree;
}

bool AMScanActionControllerScanValidator::validate(){
	if(!scanActionTree_)
		return false;
	return validateImplementation(scanActionTree_);
}

void AMScanActionControllerScanValidator::setScanActionTree(AMAction3 *scanActionTree){
	scanActionTree_ = scanActionTree;
}
