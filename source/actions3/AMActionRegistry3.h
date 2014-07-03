/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef AMACTIONREGISTRY3_H
#define AMACTIONREGISTRY3_H

#include <QObject>
#include <QHash>

#include "actions3/AMAction3.h"

/// This data structure is used by AMActionRegistry to associate an AMActionInfo with an AMAction. You should never need to use this class directly.
class AMActionInfoActionRegistration3 {
public:
	/// This constructor creates an invalid AMActionInfoActionRegistration. We need it only so that QHash has a default constructor.
 	virtual ~AMActionInfoActionRegistration3();
	AMActionInfoActionRegistration3() { actionInfoMetaObject = 0; actionMetaObject = 0; }
	/// This constructor requires the static QMetaObject* for both the AMActionInfo and AMAction, as well as a \c shortDescription and \c longDescription of the type that will be exposed to users. For ex: "REIXS XES Scan" / "This action runs a single XES scan on the REIXS beamline at a given detector energy."  You can optionally include an \c iconFileName that will be presented to users in GUIs that let them choose from a set of registered actions.
	/*! Assumes that \c actionInfoMetaObject and \c actionMetaObject are both valid pointers and of the correct type */
	AMActionInfoActionRegistration3(const QMetaObject* actionInfoMetaObject, const QMetaObject* actionMetaObject, const QString& shortDescription, const QString& longDescription, const QString& iconFileName, bool exposeToAddActionDialog);

	const QMetaObject* actionInfoMetaObject;
	const QMetaObject* actionMetaObject;
	QString shortDescription;
	QString longDescription;
	QString iconFileName;
	bool exposeToAddActionDialog;

	QString actionInfoClassName() const { return actionInfoMetaObject->className(); }
	QString actionClassName() const { return actionMetaObject->className(); }
};

/// This data structure is used by AMActionRegistry to associate an AMActionInfo with its editor widget inside Qt tree views. You should never need to use this class directly.
class AMActionInfoEditorRegistration3 {
public:
	/// Constructor requires the meta object for the AMActionInfo, and the meta object for the QWidget editor.  The default null values are required so that QHash has a default constructor.
 	virtual ~AMActionInfoEditorRegistration3();
	AMActionInfoEditorRegistration3(const QMetaObject* actionInfoMetaObjectI = 0, const QMetaObject* editorMetaObjectI = 0) { actionInfoMetaObject = actionInfoMetaObjectI; editorMetaObject = editorMetaObjectI; }

	const QMetaObject* actionInfoMetaObject;
	const QMetaObject* editorMetaObject;

	QString actionInfoClassName() const { return actionInfoMetaObject->className(); }
	QString editorClassName() const { return editorMetaObject->className(); }
};

/// This registry provides a way to associate AMActionInfo classes with their corresponding AMAction. This allows new actions to be recreated from stored AMActionInfo objects in the database. It also registers associations between AMActionInfo classes and the view widgets and delegates that can be used to edit them.
class AMActionRegistry3 : public QObject
{
	Q_OBJECT
public:
	/// The AMActionRegistry is a singleton class. As with most Acquaman singleton classes, access the only instance of it using AMActionRegistry::s().
	static AMActionRegistry3* s();

	// Public functions for registration
	//////////////////////////////////////////

	// ActionInfo -> Action
	// ====================

	/// Register an AMActionInfo / AMAction pair.  The \c shortDescription and \c longDescription describe the type of the AMAction, and will be exposed to users when browsing the available actions.  You can optionally include an \c iconFileName that will be presented to users in GUIs that let them choose from a set of registered actions.  Returns true on success, or false if \c infoMetaObject is not a meta-object for a valid AMActionInfo, or \c actionMetaObject is not a meta-object for a valid AMAction. Also returns false if the AMActionInfo class has already been registered, or if the AMAction doesn't have a Q_INVOKABLE constructor that takes an AMActionInfo* as argument.
	/*! \c shortDescription should be a human-ified version of the class name, and \c longDescription gives them more of a preview of what they're getting: for ex: "REIXS XES Scan" / "This action runs a single XES scan on the REIXS beamline at a given detector energy."   You don't need to include "blahblahblah <i>Action</i>" in the shortDescription, as this will be implied by the context.

   \note It's OK to have multiple registrations referring to the same AMAction, but an AMActionInfo can only be registered once. This is because we need to be able to instantiate an AMAction from an AMActionInfo stored in the users database, and if there were multiple registrations we wouldn't know how to resolve that ambguity.
*/
	bool registerInfoAndAction(const QMetaObject* infoMetaObject, const QMetaObject* actionMetaObject, const QString& shortDescription, const QString& longDescription, const QString& iconFileName = QString(), bool exposeToAddActionDialog = true);

	/// Register an AMActionInfo / AMAction pair.  The \c shortDescription and \c longDescription describe the type of the AMAction, and will be exposed to users when browsing the available actions.  You can optionally include an \c iconFileName that will be presented to users in GUIs that let them choose from a set of registered actions.  Returns true on success, or false if the Info and Action template classes don't inherit AMActionInfo and AMAction respectively.  Also returns false if the AMActionInfo class has already been registered, or if the AMAction doesn't have a Q_INVOKABLE constructor that takes an AMActionInfo* as argument.
	/*! \c shortDescription should be a human-ified version of the class name, and \c longDescription gives the user more of a preview of what they're getting: for ex: "REIXS XES Scan" / "This action runs a single XES scan on the REIXS beamline at a given detector energy."   You don't need to include "SomethingSomething <i>Action</i>" in the shortDescription, as this will be implied by the context.

   \note It's OK to have multiple registrations referring to the same AMAction, but an AMActionInfo can only be registered once. This is because we need to be able to instantiate an AMAction from an AMActionInfo stored in the users database, and if there were multiple registrations we wouldn't know how to resolve that ambguity.
*/
	template <class Info, class Action>
	bool registerInfoAndAction(const QString& shortDescription, const QString& longDescription, const QString& iconFileName = QString(), bool exposeToAddActionDialog = true) {
		return registerInfoAndAction( &(Info::staticMetaObject), &(Action::staticMetaObject), shortDescription, longDescription, iconFileName, exposeToAddActionDialog);
	}


	// ActionInfo -> Editor
	// ====================
	/// Register a QWidget editor widget to be used for editing an AMActionInfo subclass inside Qt treeview views.
	/*! Returns false for failure in the following siutations:
- if \c infoMetaObject doesn't describe an AMActionInfo subclass,
- if \c editorMetaObject doesn't describe a QWidget subclass,
- if there is already an editor registered for that subclass, or
- if the widget described by \c editorMetaObject doesn't have a Q_INVOKABLE constructor that accepts a pointer to this AMActionInfo.
*/
	bool registerInfoAndEditor(const QMetaObject* infoMetaObject, const QMetaObject* editorMetaObject);

	/// Register a QWidget editor widget to be used for editing an AMActionInfo subclass inside Qt treeview views.
	/*! Returns false for failure in the following siutations:
- if the first template class is not an AMActionInfo subclass, or the second is not a QWidget subclass
- if there is already an editor registered for that subclass, or
- if the widget doesn't have a Q_INVOKABLE constructor that accepts a pointer to this AMActionInfo.
*/
	template <class Info, class Editor>
	bool registerInfoAndEditor() {
		return registerInfoAndEditor(&(Info::staticMetaObject), &(Editor::staticMetaObject));
	}

	// Public functions for looking up a registration
	//////////////////////////////////////////

	// ActionInfo -> Action
	// =====================

	/// Access the registry of AMActionInfo / AMAction pairs. It is indexed by the class name of the AMActionInfo.
	QHash<QString, AMActionInfoActionRegistration3> infoAndActionRegistry() const { return actionInfo2Actions_; }

	/// Construct an AMAction instance based on the provided AMActionInfo \c info.  If successfuly, the new action takes ownership of \c info as its own.  Returns 0 if the AMActionInfo subclass has not been registered yet.
	AMAction3* createActionFromInfo(AMActionInfo3* info);


	/// Returns true if there is an action registered for the given \c info. (Means that createActionFromInfo() will be successful.)
	bool actionRegisteredForInfo(AMActionInfo3* info);

	// ActionInfo -> Editor
	// =====================

	/// Construct an editor widget for the provided AMActionInfo \c info. If successful, the new widget is configured and ready to operate on the info. Returns 0 if the AMActionInfo subclass has not been registered yet.
	QWidget* createEditorForInfo(AMActionInfo3* info);

	/// Returns true if there is an editor widget registered for the given \c info. (Means that createEditorForInfo() will return true.)
	bool editorRegisteredForInfo(AMActionInfo3* info);

signals:

public slots:

protected:

	// Internal Helper functions
	//////////////////////////////
	/// Returns true if the class with QMetaObject \c mo inherits a class with name \c className
	bool inheritsClassNamed(const QMetaObject* mo, const QString& className) const;

	/// Returns true if the class with QMetaObject \c mo inherits AMActionInfo
	bool inheritsAMActionInfo(const QMetaObject* mo) const { return inheritsClassNamed(mo, "AMActionInfo3"); }
	/// Returns true if the class with QMetaObject \c mo inherits AMAction
	bool inheritsAMAction(const QMetaObject* mo) const { return inheritsClassNamed(mo, "AMAction3"); }
	/// Returns true if the class with QMetaObject \c mo inherits QWidget
	bool inheritsQWidget(const QMetaObject* mo) const { return inheritsClassNamed(mo, "QWidget"); }

private:
	/// A hash from AMActionInfo class names to their AMActionInfoActionRegistration
	QHash<QString, AMActionInfoActionRegistration3> actionInfo2Actions_;
	/// A hash from AMActionInfo class names to their AMActionInfoEditorRegistration
	QHash<QString, AMActionInfoEditorRegistration3> actionInfo2Editors_;

	/// This is a singleton class, so the constructor is private:
 	virtual ~AMActionRegistry3();
	explicit AMActionRegistry3(QObject *parent = 0);


	/// Singleton class instance
	static AMActionRegistry3* instance_;

};

#endif // AMACTIONREGISTRY3_H
