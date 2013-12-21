/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").
Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AMActionRegistry3.h"
#include "util/AMErrorMonitor.h"

#include <QMetaMethod>

#include <QStringBuilder>

#include <QWidget>

 AMActionInfoActionRegistration3::~AMActionInfoActionRegistration3(){}
AMActionInfoActionRegistration3::AMActionInfoActionRegistration3(const QMetaObject *actionInfoMetaObjectI, const QMetaObject *actionMetaObjectI, const QString &shortDescriptionI, const QString &longDescriptionI, const QString& iconFileNameI, bool exposeToAddActionDialogI)
	: shortDescription(shortDescriptionI), longDescription(longDescriptionI), iconFileName(iconFileNameI), exposeToAddActionDialog(exposeToAddActionDialogI)
{
	actionMetaObject = actionMetaObjectI;
	actionInfoMetaObject = actionInfoMetaObjectI;

	if(iconFileName.isEmpty())
		iconFileName = ":/64x64/generic-action.png";
}

AMActionRegistry3* AMActionRegistry3::instance_ = 0;

 AMActionRegistry3::~AMActionRegistry3(){}
AMActionRegistry3::AMActionRegistry3(QObject *parent) :
	QObject(parent)
{
}



bool AMActionRegistry3::registerInfoAndAction(const QMetaObject *infoMetaObject, const QMetaObject *actionMetaObject, const QString &shortDescription, const QString &longDescription, const QString &iconFileName, bool exposeToAddActionDialog)
{
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
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, -2, "Could not register the AMAction class " % QString(actionMetaObject->className()) % " because it does not have a Q_INVOKABLE constructor that takes an " % QString(infoMetaObject->className()) % "* argument."));
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
				AMActionInfoActionRegistration3(infoMetaObject, actionMetaObject, shortDescription, longDescription, iconFileName, exposeToAddActionDialog));
	// success!
	return true;
}

bool AMActionRegistry3::actionRegisteredForInfo(AMActionInfo3 *info) {
	return actionInfo2Actions_.contains(QString(info->metaObject()->className()));
}

AMAction3 * AMActionRegistry3::createActionFromInfo(AMActionInfo3 *info)
{
	if(!info)
		return 0;

	QString infoClassName = QString(info->metaObject()->className());

	// is the info class registered?
	if(!actionInfo2Actions_.contains(infoClassName))
		return 0;

	const QMetaObject* actionMetaObject = actionInfo2Actions_.value(infoClassName).actionMetaObject;

	// this usage of QGenericArgument (instead of the Q_ARG macro) is technically unsupported, but the only way to do it because we don't have the type at compile-time.
	return qobject_cast<AMAction3*>(actionMetaObject->newInstance(
						QGenericArgument(infoClassName.append("*").toAscii().constData(),
								 static_cast<const void*>(&info))));
}

AMActionRegistry3 * AMActionRegistry3::s()
{
	if(!instance_)
		instance_ = new AMActionRegistry3();
	return instance_;
}

bool AMActionRegistry3::inheritsClassNamed(const QMetaObject *mo, const QString& className) const
{
	const QMetaObject* superClass = mo;
	bool doesInheritClass;
	do {
		doesInheritClass = (superClass->className() == className);
	}
	while( (superClass=superClass->superClass()) && doesInheritClass == false );

	return doesInheritClass;
}

bool AMActionRegistry3::registerInfoAndEditor(const QMetaObject *infoMetaObject, const QMetaObject *editorMetaObject) {
	if(!infoMetaObject)
		return false;
	if(!editorMetaObject)
		return false;

	// check that the alleged info inherits AMActionInfo
	if(!inheritsAMActionInfo(infoMetaObject)) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, -1, "Could not register the AMActionInfo class " % QString(infoMetaObject->className()) % " because it does not inherit AMActionInfo."));
		return false;
	}
	// check that the alleged editor inherits QWidget
	if(!inheritsQWidget(editorMetaObject)) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, -2, "Could not register the Editor class " % QString(editorMetaObject->className()) % " because it does not inherit QWidget."));
		return false;
	}

	// check for Q_INVOKABLE constructor in the editor that takes a pointer to this AMActionInfo. If this constructor doesn't exist, there won't be any way we can use this registration to construct an editor for the info.
	bool hasSuitableConstructor = false;
	for(int i=0, cc=editorMetaObject->constructorCount(); i<cc; i++) {
		QList<QByteArray> parameterTypes = editorMetaObject->constructor(i).parameterTypes();
		if(parameterTypes.count() == 1 &&
				parameterTypes.at(0) == QByteArray(infoMetaObject->className()).append("*")) {
			hasSuitableConstructor = true;
			break;
		}
	}


	if(!hasSuitableConstructor) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, -2, "Could not register the Editor class " % QString(editorMetaObject->className()) % " because it does not have a Q_INVOKABLE constructor that takes an " % QString(infoMetaObject->className()) % "* argument."));
		return false;
	}


	// already registered?
	if(actionInfo2Editors_.contains(QString(infoMetaObject->className()))) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, -2, "Could not register the AMActionInfo class " % QString(infoMetaObject->className()) % " because it is already registered."));
		return false;
	}

	// insert into registry
	actionInfo2Editors_.insert(
				QString(infoMetaObject->className()),
				AMActionInfoEditorRegistration3(infoMetaObject, editorMetaObject));
	// success!
	return true;
}

bool AMActionRegistry3::editorRegisteredForInfo(AMActionInfo3 *info)
{
	return actionInfo2Editors_.contains(QString(info->metaObject()->className()));
}

QWidget * AMActionRegistry3::createEditorForInfo(AMActionInfo3 *info)
{
	if(!info)
		return 0;

	QString infoClassName = QString(info->metaObject()->className());

	// is the info class registered?
	if(!actionInfo2Editors_.contains(infoClassName))
		return 0;

	const QMetaObject* editorMetaObject = actionInfo2Editors_.value(infoClassName).editorMetaObject;

	// this usage of QGenericArgument (instead of the Q_ARG macro) is technically unsupported, but the only way to do it because we don't know the type at compile-time.
	return qobject_cast<QWidget*>(editorMetaObject->newInstance(
						  QGenericArgument(infoClassName.append("*").toAscii().constData(),
								   static_cast<const void*>(&info))));
}
 AMActionInfoEditorRegistration3::~AMActionInfoEditorRegistration3(){}
