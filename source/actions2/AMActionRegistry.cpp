#include "AMActionRegistry.h"
#include "util/AMErrorMonitor.h"

#include <QMetaMethod>

#include <QStringBuilder>

AMActionInfoActionRegistration::AMActionInfoActionRegistration(const QMetaObject *actionInfoMetaObjectI, const QMetaObject *actionMetaObjectI, const QString &shortDescriptionI, const QString &longDescriptionI, const QString& iconFileNameI)
	: shortDescription(shortDescriptionI), longDescription(longDescriptionI), iconFileName(iconFileNameI)
{
	actionMetaObject = actionMetaObjectI;
	actionInfoMetaObject = actionInfoMetaObjectI;

	if(iconFileName.isEmpty())
		iconFileName = ":/64x64/generic-action.png";
}

AMActionRegistry* AMActionRegistry::instance_ = 0;

AMActionRegistry::AMActionRegistry(QObject *parent) :
	QObject(parent)
{
}



bool AMActionRegistry::registerInfoAndAction(const QMetaObject *infoMetaObject, const QMetaObject *actionMetaObject, const QString &shortDescription, const QString &longDescription, const QString &iconFileName) {

	if(!infoMetaObject)
		return false;
	if(!actionMetaObject)
		return false;

	// check that the alleged info inherits AMActionInfo
	if(!inheritsAMActionInfo(infoMetaObject)) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, -1, "Could not register the AMActionInfo class " % QString(infoMetaObject->className()) % " because it does not inherit AMActionInfo."));
		return false;
	}
	// check that the alleged action inherits AMAction
	if(!inheritsAMAction(actionMetaObject)) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, -2, "Could not register the AMAction class " % QString(actionMetaObject->className()) % " because it does not inherit AMAction."));
		return false;
	}

	// check for Q_INVOKABLE constructor in the AMAction that takes a pointer to this AMActionInfo. If this constructor doesn't exist, there won't be any way we can use this registration to construct an action from the info.
	bool hasSuitableConstructor = false;
	for(int i=0, cc=actionMetaObject->constructorCount(); i<cc; i++) {
		QList<QByteArray> parameterTypes = actionMetaObject->constructor(i).parameterTypes();
		if(parameterTypes.count() == 1 &&
				parameterTypes.at(0) == QByteArray(infoMetaObject->className()).append("*")) {
			hasSuitableConstructor = true;
			break;
		}
	}

	if(!hasSuitableConstructor) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, -2, "Could not register the AMAction class " % QString(actionMetaObject->className()) % " because it does not have a Q_INVOKABLE constructor that takes an " % QString(actionMetaObject->className()) % "* argument."));
		return false;
	}


	// already registered?
	if(actionInfo2Actions_.contains(QString(infoMetaObject->className()))) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, -2, "Could not register the AMActionInfo class " % QString(infoMetaObject->className()) % " because it is already registered."));
		return false;
	}

	// insert into registry
	actionInfo2Actions_.insert(
				QString(infoMetaObject->className()),
				AMActionInfoActionRegistration(infoMetaObject, actionMetaObject, shortDescription, longDescription, iconFileName));
	// success!
	return true;
}

AMAction * AMActionRegistry::createActionFromInfo(AMActionInfo *info)
{
	if(!info)
		return 0;

	QString infoClassName = QString(info->metaObject()->className());

	// is the info class registered?
	if(!actionInfo2Actions_.contains(infoClassName))
		return 0;

	const QMetaObject* actionMetaObject = actionInfo2Actions_.value(infoClassName).actionMetaObject;

	// this usage of QGenericArgument (instead of the Q_ARG macro) is technically unsupported, but the only way to do it because we don't have the type at compile-time.
	return qobject_cast<AMAction*>(actionMetaObject->newInstance(
									   QGenericArgument(infoClassName.append("*").toAscii().constData(),
														static_cast<const void*>(&info))));
}

AMActionRegistry * AMActionRegistry::s()
{
	if(!instance_)
		instance_ = new AMActionRegistry();
	return instance_;
}

bool AMActionRegistry::inheritsAMActionInfo(const QMetaObject *mo) const
{
	const QMetaObject* superClass = mo;
	bool inheritsActionInfo;
	do {
		inheritsActionInfo = (superClass->className() == QString("AMActionInfo"));
	}
	while( (superClass=superClass->superClass()) && inheritsActionInfo == false );

	return inheritsActionInfo;
}

bool AMActionRegistry::inheritsAMAction(const QMetaObject *mo) const
{
	const QMetaObject* superClass = mo;
	bool inheritsAction;
	do {
		inheritsAction = (superClass->className() == QString("AMAction"));
	}
	while( (superClass=superClass->superClass()) && inheritsAction == false );

	return inheritsAction;
}

