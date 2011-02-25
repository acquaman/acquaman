#ifndef AMDEFERREDFUNCTIONCALL_H
#define AMDEFERREDFUNCTIONCALL_H

#include <QObject>
#include <QTimer>
#include <QEvent>
#include <QCoreApplication>

/// This class provides a convenient way to schedule something to run after all currently-running functions return... ie: when flow control returns to the Qt event loop. It also provides the handy optimization of combining several repeated function calls into one call.
/*! (For example, if you need to do an expensive computation whenever any one of a set of values is changed, and the user might be setting multiple values in one function call, it might be beneficial to wait for all the values to change, and then do your computation once.)

There are two ways to use this class.  You can inherit it, and re-implement execute() to do your dirty business. Alternatively, you can simply connect the executed() signal to your desired slots.    Call schedule() one or more times to schedule the function call(s) to execute.  Multiple calls to schedule() within one run loop will only result in one execution.

Both execute() and the executed() signal will be triggered on every run loop, if you've called schedule() one or more times in the previous run loop.*/
class AMDeferredFunctionCall : public QObject {
	Q_OBJECT

public:
	AMDeferredFunctionCall(QObject* parent = 0);

public slots:
	/// Schedule the function call(s) for execution.
	void schedule() {
		if(!scheduled_) {
			QCoreApplication::postEvent(this, new QEvent(eventType_), Qt::HighEventPriority);
			scheduled_ = true;
		}
	}

	/// Run the function call(s). The base class version does nothing. You can re-implement this to do work required in a custom class (or alternatively, connect the executed() signal to your desired slots.)
	virtual void execute() {
	}

signals:
	/// Emitted when control returns to the Qt event loop and schedule() has been called at least once in the past run loop.
	void executed();

protected:
	bool scheduled_;
	static QEvent::Type eventType_;

	bool event(QEvent* e) {
		if(e->type() == eventType_) {
			e->accept();
			scheduled_ = false;
			emit executed();
			execute();
			return true;
		}
		else
			return false;
	}
};

#endif // AMDEFERREDFUNCTIONCALL_H
