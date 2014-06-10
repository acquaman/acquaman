#ifndef AMSCANACTIONCONTROLLERSCANOPTIMIZER_H
#define AMSCANACTIONCONTROLLERSCANOPTIMIZER_H

#include <QObject>

#include "acquaman/AMScanActionTreeSupport.h"

class AMAction3;

class AMScanActionControllerScanOptimizer : public QObject
{
Q_OBJECT
public:
 	virtual ~AMScanActionControllerScanOptimizer();
	AMScanActionControllerScanOptimizer(AMAction3 *scanActionTree = 0, QObject *parent = 0);

public slots:
	void optimize();

	void setScanActionTree(AMAction3 *scanActionTree);

protected:
	virtual void optimizeImplementation(AMAction3 *scanActionTree) = 0;

protected:
	AMAction3 *scanActionTree_;
};

#endif // AMSCANACTIONCONTROLLERSCANOPTIMIZER_H
