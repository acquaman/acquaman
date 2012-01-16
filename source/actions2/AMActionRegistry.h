#ifndef AMACTIONREGISTRY_H
#define AMACTIONREGISTRY_H

#include <QObject>
#include <QHash>

#include "actions2/AMAction.h"

/// This data structure is used by AMActionRegistry to associate an AMActionInfo with an AMAction. You should never need to use this class directly.
class AMActionInfoActionRegistration {
public:
	/// This constructor creates an invalid AMActionInfoRegistration. We need it only so that QHash has a default constructor.
	AMActionInfoActionRegistration() { actionInfoMetaObject = 0; actionMetaObject = 0; }
	/// This constructor requires the static QMetaObject* for both the AMActionInfo and AMAction, as well as a \c shortDescription and \c longDescription of the type that will be exposed to users. For ex: "REIXS XES Scan" / "This action runs a single XES scan on the REIXS beamline at a given detector energy."  You can optionally include an \c iconFileName that will be presented to users in GUIs that let them choose from a set of registered actions.
	/*! Assumes that \c actionInfoMetaObject and \c actionMetaObject are both valid pointers and of the correct type */
	AMActionInfoActionRegistration(const QMetaObject* actionInfoMetaObject, const QMetaObject* actionMetaObject, const QString& shortDescription, const QString& longDescription, const QString& iconFileName);

	const QMetaObject* actionInfoMetaObject;
	const QMetaObject* actionMetaObject;
	QString shortDescription;
	QString longDescription;
	QString iconFileName;

	QString actionInfoClassName() const { return actionInfoMetaObject->className(); }
	QString actionClassName() const { return actionMetaObject->className(); }
};

/// This registry provides a way to associate AMActionInfo classes with their corresponding AMAction. This allows new actions to be recreated from stored AMActionInfo objects in the database. It also registers associations between AMActionInfo classes and the view widgets and delegates that can be used to edit them.
class AMActionRegistry : public QObject
{
    Q_OBJECT
public:
	/// The AMActionRegistry is a singleton class. As with most Acquaman singleton classes, access the only instance of it using AMActionRegistry::s().
	static AMActionRegistry* s();

	// Public functions for registration
	//////////////////////////////////////////

	/// Register an AMActionInfo / AMAction pair.  The \c shortDescription and \c longDescription describe the type of the AMAction, and will be exposed to users when browsing the available actions.  You can optionally include an \c iconFileName that will be presented to users in GUIs that let them choose from a set of registered actions.  Returns true on success, or false if \c infoMetaObject is not a meta-object for a valid AMActionInfo, or \c actionMetaObject is not a meta-object for a valid AMAction. Also returns false if the AMActionInfo class has already been registered, or if the AMAction doesn't have a Q_INVOKABLE constructor that takes an AMActionInfo* as argument.
	/*! \c shortDescription should be a human-ified version of the class name, and \c longDescription gives them more of a preview of what they're getting: for ex: "REIXS XES Scan" / "This action runs a single XES scan on the REIXS beamline at a given detector energy."   You don't need to include "blahblahblah <i>Action</i>" in the shortDescription, as this will be implied by the context.

	  \note It's OK to have multiple registrations referring to the same AMAction, but an AMActionInfo can only be registered once. This is because we need to be able to instantiate an AMAction from an AMActionInfo stored in the users database, and if there were multiple registrations we wouldn't know how to resolve that ambguity.
*/
	bool registerInfoAndAction(const QMetaObject* infoMetaObject, const QMetaObject* actionMetaObject, const QString& shortDescription, const QString& longDescription, const QString& iconFileName = QString());

	/// Register an AMActionInfo / AMAction pair.  The \c shortDescription and \c longDescription describe the type of the AMAction, and will be exposed to users when browsing the available actions.  You can optionally include an \c iconFileName that will be presented to users in GUIs that let them choose from a set of registered actions.  Returns true on success, or false if the Info and Action template classes don't inherit AMActionInfo and AMAction respectively.  Also returns false if the AMActionInfo class has already been registered, or if the AMAction doesn't have a Q_INVOKABLE constructor that takes an AMActionInfo* as argument.
	/*! \c shortDescription should be a human-ified version of the class name, and \c longDescription gives the user more of a preview of what they're getting: for ex: "REIXS XES Scan" / "This action runs a single XES scan on the REIXS beamline at a given detector energy."   You don't need to include "SomethingSomething <i>Action</i>" in the shortDescription, as this will be implied by the context.

	  \note It's OK to have multiple registrations referring to the same AMAction, but an AMActionInfo can only be registered once. This is because we need to be able to instantiate an AMAction from an AMActionInfo stored in the users database, and if there were multiple registrations we wouldn't know how to resolve that ambguity.
*/
	template <class Info, class Action>
	bool registerInfoAndAction(const QString& shortDescription, const QString& longDescription, const QString& iconFileName = QString()) {
		return registerInfoAndAction( &(Info::staticMetaObject), &(Action::staticMetaObject), shortDescription, longDescription, iconFileName );
	}


	// Public functions for looking up a registration
	//////////////////////////////////////////

	/// Access the registry of AMActionInfo / AMAction pairs. It is indexed by the class name of the AMActionInfo.
	QHash<QString, AMActionInfoActionRegistration> infoAndActionRegistry() const { return actionInfo2Actions_; }

	/// Construct an AMAction instance based on the provided AMActionInfo \c info.  If successfuly, the new action takes ownership of \c info as its own.  Returns 0 if the AMActionInfo subclass has not been registered yet.
	AMAction* createActionFromInfo(AMActionInfo* info);


signals:

public slots:

protected:

	// Internal Helper functions
	//////////////////////////////
	/// Returns true if the class with QMetaObject \c mo inherits AMActionInfo
	bool inheritsAMActionInfo(const QMetaObject* mo) const;
	/// Returns true if the class with QMetaObject \c mo inherits AMAction
	bool inheritsAMAction(const QMetaObject* mo) const;

private:
	/// A hash from AMActionInfo class names to their AMActionInfoActionRegistration
	QHash<QString, AMActionInfoActionRegistration> actionInfo2Actions_;

	/// This is a singleton class, so the constructor is private:
	explicit AMActionRegistry(QObject *parent = 0);


	/// Singleton class instance
	static AMActionRegistry* instance_;

};

#endif // AMACTIONREGISTRY_H
