#ifndef AMLISTACTIONSCANOPTIMIZER_H
#define AMLISTACTIONSCANOPTIMIZER_H

#include "acquaman/AMScanActionControllerScanOptimizer.h"

class AMEmptyListScanOptimizer : public AMScanActionControllerScanOptimizer
{
Q_OBJECT
public:
	AMEmptyListScanOptimizer(AMAction3 *scanActionTree = 0, QObject *parent = 0);

protected:
	virtual void optimizeImplementation(AMAction3 *scanActionTree);
};

class AMSingleElementListOptimizer : public AMScanActionControllerScanOptimizer
{
Q_OBJECT
public:
	AMSingleElementListOptimizer(AMAction3 *scanActionTree = 0, QObject *parent = 0);

protected:
	virtual void optimizeImplementation(AMAction3 *scanActionTree);
};

#endif // AMLISTACTIONSCANOPTIMIZER_H
