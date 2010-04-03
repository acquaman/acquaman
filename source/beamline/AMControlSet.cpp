#include "AMControlSet.h"

AMControlSet::AMControlSet(QObject *parent) :
    QObject(parent)
{
}

bool AMControlSet::addControl(AMControl* ctrl) {
	if(ctrls_.contains(ctrl))
        return false;
    ctrls_.append(ctrl);
    return true;
}

bool AMControlSet::removeControl(AMControl* ctrl) {
    return ctrls_.removeOne(ctrl);
}

QMap<double, double> AMControlOptimization::curve(QList<QVariant> stateParameters, QList<AMRegion*> contextParameters) {
    QMap<double, double> noMap;
    Q_UNUSED(stateParameters);
    Q_UNUSED(contextParameters);
    return noMap;
}
