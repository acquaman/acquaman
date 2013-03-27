#ifndef AMDETECTORTRIGGERSOURCESCANOPTIMIZER_H
#define AMDETECTORTRIGGERSOURCESCANOPTIMIZER_H

#include "acquaman/AMScanActionControllerScanOptimizer.h"

class AMDetectorTriggerSourceScanOptimizer : public AMScanActionControllerScanOptimizer
{
Q_OBJECT
public:
	AMDetectorTriggerSourceScanOptimizer(AMAction3 *scanActionTree, QObject *parent = 0);

protected:
	virtual void optimizeImplementation(AMAction3 *scanActionTree);

	QList<AMAction3*> findActionsNamed(AMAction3 *rootAction, const QString &name);
};

#endif // AMDETECTORTRIGGERSOURCESCANOPTIMIZER_H
