#ifndef AMSCANACTIONCONTROLLERSCANOPTIMIZER_H
#define AMSCANACTIONCONTROLLERSCANOPTIMIZER_H

#include <QObject>

class AMAction3;

namespace AMScanOptimizerSupport{
	QList<AMAction3*> findActionsNamed(AMAction3 *rootAction, const QString &name);

	QList<AMAction3*> findActionsOfType(AMAction3 *rootAction, AMAction3 *typeAction);
}

class AMScanActionControllerScanOptimizer : public QObject
{
Q_OBJECT
public:
	AMScanActionControllerScanOptimizer(AMAction3 *scanActionTree, QObject *parent = 0);

	AMAction3* scanActionTree();

public slots:
	void optimize();

protected:
	virtual void optimizeImplementation(AMAction3 *scanActionTree) = 0;

protected:
	AMAction3 *scanActionTree_;
};

#endif // AMSCANACTIONCONTROLLERSCANOPTIMIZER_H
