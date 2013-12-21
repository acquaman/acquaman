#ifndef AMNESTEDAXISTYPEVALIDATOR_H
#define AMNESTEDAXISTYPEVALIDATOR_H

#include "acquaman/AMScanActionControllerScanValidator.h"

class AMNestedAxisTypeValidator : public AMScanActionControllerScanValidator
{
Q_OBJECT
public:
 	virtual ~AMNestedAxisTypeValidator();
	AMNestedAxisTypeValidator(AMAction3 *scanActionTree = 0, QObject *parent = 0);

protected:
	virtual bool validateImplementation(AMAction3 *scanActionTree);
};

#endif // AMNESTEDAXISTYPEVALIDATOR_H
