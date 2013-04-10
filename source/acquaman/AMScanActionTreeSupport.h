#ifndef AMSCANACTIONTREESUPPORT_H
#define AMSCANACTIONTREESUPPORT_H

#include <QList>
class AMAction3;

namespace AMScanActionTreeSupport{
	QList<AMAction3*> findActionsWhereNameContains(AMAction3 *rootAction, const QString &searchString);
	QList<AMAction3*> findActionsNamed(AMAction3 *rootAction, const QString &name);

	QList<AMAction3*> findActionsOfType(AMAction3 *rootAction, AMAction3 *typeAction);
}

#endif // AMSCANACTIONTREESUPPORT_H
