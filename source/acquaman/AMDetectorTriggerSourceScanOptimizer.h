#ifndef AMDETECTORTRIGGERSOURCESCANOPTIMIZER_H
#define AMDETECTORTRIGGERSOURCESCANOPTIMIZER_H

#include "acquaman/AMScanActionControllerScanOptimizer.h"

class AMDetectorTriggerSourceScanOptimizer : public AMScanActionControllerScanOptimizer
{
Q_OBJECT
public:
 	virtual ~AMDetectorTriggerSourceScanOptimizer();
	AMDetectorTriggerSourceScanOptimizer(AMAction3 *scanActionTree = 0, QObject *parent = 0);

protected:
	virtual void optimizeImplementation(AMAction3 *scanActionTree);
};

#endif // AMDETECTORTRIGGERSOURCESCANOPTIMIZER_H
