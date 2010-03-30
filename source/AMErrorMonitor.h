#ifndef ERRORMONITOR_H
#define ERRORMONITOR_H

#include <QObject>
#include <QMap>
#include <QPair>
#include <QMetaType>

class QSystemTrayIcon;

/// This class encapsulates an error message (who it's from, the level or "severity", an error code defined by the originator, and a description)
/*! Error levels are defined as:

	Information: You might like to know...
	Alert: Something weird happened, but don't worry... I handled it.
	Serious: Something happened, and what you were hoping for isn't going to happen anymore.
	Debug: Way more information than you wanted to know. (You can turn this off.)
*/

class AMErrorReport {

public:
	enum Level {Information, Alert, Serious, Debug} ;

	AMErrorReport(QObject* src=0, Level l=Alert, int code=0, const QString& desc = "") : source(src), level(l), errorCode(code), description(desc) {}

	QObject* source;
	Level level;
	int errorCode;
	QString description;
};

Q_DECLARE_METATYPE(AMErrorReport)


/// This class provides a system-wide error and notification manager.  Other classes can report errors from anywhere using AMErrorMon::report(AMErrorReport()).
/*! By centralizing all error reporting in one place, it makes it possible for any object to subscribe to receive notification of errors created anywhere in the program.  You can request to receive errors from a specific object instance, class, or error code.
Subscribers must provide the name of an error-handling slot that looks like: <code>void errorSlot(AMErrorReport e)</code>.  Whenever an error happens, that slot will be called with an AMErrorReport containing the relevant information.


Classes can also subscribe to be notified when specific errors or notifications occur, using AMErrorMon::subscribeToObject(), AMErrorMon::subscribeToClass(), and AMErrorMon::subscribeToCode().
These three versions will notify the subscribing object whenever an error is generated:
- by a specific object instance (subscribeToObject)
- by any member of a specific class (subscribeToClass), or
- with a matching specific error code (subscribeToCode)

When subscribing to receive these notifications, the subscribing object must provide the name of the slot that will be called when the matching error occurs.  This slot must take one argument: an AMErrorReport.
For example, supposer that <code>MySweetObject* myObject</code> wants to subscribe to all notifications with the error code 42.  It must have a slot that looks like:

\code
class MySweetObject : public QObject {
Q_OBJECT
public slots:
void handleError42sSlot(AMErrorReport e);
void handleStinkyClassErrors(AMErrorReport e);
}
\endcode

and it can then subscribe as:

\code
MySweetObject* myObject = new MySweetObject();
AMErrorMon::subscribeToCode(42, myObject, "handleError42sSlot");
\endcode

Now whenever any error with code 42 is reported anywhere within the program, myObject's slot will be called.  The AMErrorReport structure will provide information on who originated the error, the level of seriousness, and a nice description of it.

The same object can subscribe to different errors and send then to different slots. For example, we could also request all errors from any StinkyClass objects:

\code
AMErrorMon::subscribeToClass("StinkyClass", myObject, "handleStinkyClassErrors");
\endcode

Unsubscribing from errors is also possible:

\code
AMErrorMon::unsubscribe(myObject, "handleError42sSlot")
\endcode

will remove any subscriptions that would have called the \c handleError42sSlot(AMErrorCode) slot within myObject.  To unsubscribe yourself completely (all slots), use

\code
AMErrorMon::unsubscribe(myObject);
\endcode

\todo What happens if an object is deleted after subscribing?
\note All of the functions in this class are thread-safe, and can be called from any thread.  Notifications will also be delivered to any thread, using Qt's queued slot-calling mechanism when the subscribing object is in a different thread than the ErrorMon. (This is why slots must be used, instead of callback functions.)

*/
class AMErrorMon : public QObject {

	Q_OBJECT

public:

	/// Subscribe to all errors from object 'originator'
	static void subscribeToObject(QObject* originator, QObject* notifyMe, const char* errorSlot) {
		mon()->subscribeToObjectI(originator, notifyMe, errorSlot);
	}


	/// Subscribe to all errors from this class:
	static void subscribeToClass(const QString& className, QObject* notifyMe, const char* errorSlot) {
		mon()->subscribeToClassI(className, notifyMe, errorSlot);
	}


	/// Subscribe to all errors that have code 'errorCode'
	static void subscribeToCode(int errorCode, QObject* notifyMe, const char* errorSlot) {
		mon()->subscribeToCodeI(errorCode, notifyMe, errorSlot);
	}


	/// Unsubscribe the \c notifyMe object from all subscriptions that would call \c errorSlot. If \c errorSlot==0, it unsubscribes notifyMe from everything.
	static void unsubscribe(QObject* notifyMe, const char* errorSlot = 0) {
		mon()->unsubscribeI(notifyMe, errorSlot);
	}


	/// Report an error:
	static void report(const AMErrorReport& e) {
		mon()->reportI(e);
	}

	/// Enable or disable debug-level notifications:
	static void enableDebugNotifications(bool debugEnabled) {
		mon()->debugEnabled_ = debugEnabled;
	}


signals:
	/// emitted for all errors:
	void error(const AMErrorReport& e);
	/// Emitted for all information notifications:
	void information(const AMErrorReport& e);
	/// Emitted for all alert notifications:
	void alert(const AMErrorReport& e);
	/// Emitted for all serious notifications:
	void serious(const AMErrorReport& e);
	/// Emitted for all debug notifications. Can be disabled by calling enableDebugNotifications(false);
	void debug(const AMErrorReport& e);

public slots:
	/// Report an error:
	void reportI(const AMErrorReport& e);

protected:

	/// Subscribe to all errors from object 'originator'
	void subscribeToObjectI(QObject* originator, QObject* notifyMe, const char* errorSlot);

	/// Subscribe to all errors from this class:
	void subscribeToClassI(const QString& className, QObject* notifyMe, const char* errorSlot);

	/// Subscribe to all errors that have code 'errorCode'
	void subscribeToCodeI(int errorCode, QObject* notifyMe, const char* errorSlot);

	/// Unsubscribe the \c notifyMe object from all subscriptions that would call \c errorSlot. If \c errorSlot==0, it unsubscribes notifyMe from everything.
	void unsubscribeI(QObject* notifyMe, const char* errorSlot = 0);

	/// Get access to the single instance of this class:
	static AMErrorMon* mon() {
		if(!instance_)
			instance_ = new AMErrorMon();
		return instance_;
	}

	/// Maps subscriptions from originating objects to <Object, Slot> recipients.
	QMap<QObject*, QPair<QObject*, QString> > objectSubs_;
	/// Maps subs. from originating classes to <Object, Slot> recipients.
	QMap<QString, QPair<QObject*, QString> > classSubs_;
	/// Maps subs. from error codes to <QObject, Slot> recipients.
	QMap<int, QPair<QObject*, QString> > codeSubs_;

private:
	/// Singleton class.  Private Constructor:
	explicit AMErrorMon();

	/// class-wide instance variable
	static AMErrorMon* instance_;

	/// SystemTrayIcon object used to display serious error notifications very visibly on-screen.
	QSystemTrayIcon* sicon_;

	/// Whether we display debug messages:
	bool debugEnabled_;
};


#endif // ERRORMONITOR_H
