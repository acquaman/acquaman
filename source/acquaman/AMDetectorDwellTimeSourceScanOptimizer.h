#ifndef AMDETECTORDWELLTIMESOURCESCANOPTIMIZER_H
#define AMDETECTORDWELLTIMESOURCESCANOPTIMIZER_H

#include "acquaman/AMScanActionControllerScanOptimizer.h"

class AMDetectorDwellTimeSourceScanOptimizer : public AMScanActionControllerScanOptimizer
{
Q_OBJECT
public:
 	virtual ~AMDetectorDwellTimeSourceScanOptimizer();
	AMDetectorDwellTimeSourceScanOptimizer(AMAction3 *scanActionTree = 0, QObject *parent = 0);

protected:
	virtual void optimizeImplementation(AMAction3 *scanActionTree);
};

#endif // AMDETECTORDWELLTIMESOURCESCANOPTIMIZER_H
