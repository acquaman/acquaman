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

class ErrorReport {

public:
	enum Level {Information, Alert, Serious, Debug} ;

	ErrorReport(QObject* src=0, Level l=Alert, int code=0, const QString& desc = "") : source(src), level(l), errorCode(code), description(desc) {}

	QObject* source;
	Level level;
	int errorCode;
	QString description;
};

Q_DECLARE_METATYPE(ErrorReport)


class ErrorMon : public QObject {

Q_OBJECT

public:
	/// By centralizing all error reporting in one place, it makes it possible for any object to subscribe to receive notification
	/// of errors created anywhere in the program.  You can request to receive errors from a specific object instance, class, or error code.
	/// Subscribers must provide the name of an error-handling slot that looks like: void errorSlot(ErrorReport e);
		/// Whenever an error happens, that slot will be called with an ErrorReport containing the relevant information.

	/// Subscribe to all errors from object 'originator'
	void subscribeToObjectI(QObject* originator, QObject* notifyMe, const char* errorSlot);
	static void subscribeToObject(QObject* originator, QObject* notifyMe, const char* errorSlot) {
		mon()->subscribeToObjectI(originator, notifyMe, errorSlot);
	}


	/// Subscribe to all errors from this class:
	void subscribeToClassI(const QString& className, QObject* notifyMe, const char* errorSlot);
	static void subscribeToClass(const QString& className, QObject* notifyMe, const char* errorSlot) {
		mon()->subscribeToClassI(className, notifyMe, errorSlot);
	}


	/// Subscribe to all errors that have code 'errorCode'
	void subscribeToCodeI(int errorCode, QObject* notifyMe, const char* errorSlot);
	static void subscribeToCode(int errorCode, QObject* notifyMe, const char* errorSlot) {
		mon()->subscribeToCodeI(errorCode, notifyMe, errorSlot);
	}


	/// Unsubscribe from everything:
	void unsubscribeI(QObject* notifyMe);
	static void unsubscribe(QObject* notifyMe) {
		mon()->unsubscribeI(notifyMe);
	}


	/// Report an error:
	static void report(const ErrorReport& e) {
		mon()->reportI(e);
	}

	/// Get access to the single instance of this class:
	static ErrorMon* mon() {
		if(!instance_)
			instance_ = new ErrorMon();
		return instance_;
	}

signals:
	// emitted for all errors:
	void error(const ErrorReport& e);
	/// Emitted for each individual level:
	void information(const ErrorReport& e);
	void alert(const ErrorReport& e);
	void serious(const ErrorReport& e);
	void debug(const ErrorReport& e);

public slots:
	/// Report an error:
	void reportI(const ErrorReport& e);

protected:
	QMap<QObject*, QPair<QObject*, QString> > objectSubs_;
	QMap<QString, QPair<QObject*, QString> > classSubs_;
	QMap<int, QPair<QObject*, QString> > codeSubs_;

private:
	// Singleton class.  Private Constructor:
	explicit ErrorMon();

	static ErrorMon* instance_;

	QSystemTrayIcon* sicon_;
};


#endif // ERRORMONITOR_H
