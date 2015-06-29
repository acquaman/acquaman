#ifndef AMGCS2ASYNCCOMMAND_H
#define AMGCS2ASYNCCOMMAND_H

#define TIMER_CHECK_INTERVAL_MS 50

#include <QObject>
#include "AMGCS2Command.h"
/*!
  * Addition to the GCS2Command which allows for an observer to be tied to the
  * completed and failed signals which indicate the command has been fully completed
  * (i.e. not just started correctly).
  */
class AMGCS2AsyncCommand : public QObject, public AMGCS2Command
{
    Q_OBJECT
public:
	/*!
	  * Creates an instance of an asynchronous command which will emit the completed
	  * or failed signals when the command comes to a stop. The timer will automatically
	  * emit the failed signal and cease its checking mechanism when timeoutMs milliseconds
	  * have elapsed.
	  */
	explicit AMGCS2AsyncCommand(double timeoutMs = 20);

signals:
	/*!
	  * Signal indicating that this command has successfully finished
	  */
	void completed();

	/*!
	  * Signal indicating that this command has finished in a failed state.
	  */
	void failed();

	/*!
	  * Overrides the synchronous GCS2Command implementation of run. Begins the
	  * mechanism for checking if the command has done.
	  */
	virtual void run();
public slots:

protected:

	enum RunningState {
		NotFinished,
		Completed,
		Failed
	};

	/*!
	  * Runs the implementation specific isFinishedImplemetation function ,emits
	  * the relevant signal indicated by the implementations return value (if required)
	  * and ceases the mechanism for checking if the command has done.
	  */
	void timerEvent(QTimerEvent *);

	/*!
	  * Class specific implementation which indicates when the particular asynchronous
	  * command has done, and what state it finished in.
	  * \returns NotFinsied if the command is still on-going, Completed if the
	  * command has finished successfully, Failed if the command has finished but
	  * not successfully.
	  */
	virtual RunningState isFinishedImplementation() = 0;

	int timerId_;
	double timeoutMs_;
};

#endif // AMGCS2ASYNCCOMMAND_H
