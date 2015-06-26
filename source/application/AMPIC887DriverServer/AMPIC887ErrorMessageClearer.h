#ifndef AMPIC887ERRORMESSAGECLEARER_H
#define AMPIC887ERRORMESSAGECLEARER_H

#include <QObject>

/*!
  * \brief A helper class for use in the STP (stop) and HLT (halt smoothly) commands
  * to delay clearing the error message for some specified length of time. Call
  * start() passing a number of milliseconds to schedule clearning of the error
  * message to take place no earlier than the provided time.
  *
  * There appears to be a bug either in the PI_GCS2_DLL library, or with the controller
  * itself, which causes a command to be rejected (with a timeout error) if it is
  * issued before the axes have fully come to a stop. This causes problems because
  * both STP and HLT commands set the error state of the controller, and the library
  * is coded in such a way that commands always fail if an error code exists in
  * the controller. This means it is required for our STP and HLT implementations
  * to clear this error message as part of their procedure, or else the next issued
  * command will always fail. These two problems combine to mean that there needs
  * to be a set delay each time either command is called (in order to give the axes
  * time to come to a stand-still) before the error can be cleared. This class
  * exists to allow us to schedule this clearing of the error message after some
  * defined length of time.
  *
  * NOTE: Because this class has to outlive the command class which requires it,
  * it must be allocated on the heap. However in order to ensure that it does not
  * create a memory leak it must call deleteLater() on itself after clearing the
  * error message. As such creating a stack allocated instance of the class will
  * still cause deleteLater() to be called - resulting in undefined behaviour.
  */
class AMPIC887ErrorMessageClearer : public QObject
{
	Q_OBJECT
public:
	/*!
	  * Creates a new instance of an error message clearer which will clear the
	  * error message of the controller with the provided id.
	  * \param controllerId ~ The id of the controller whose error message is to
	  * be scheduled for clearing.
	  */
	AMPIC887ErrorMessageClearer(int controllerId);

	/*!
	  * Virtual destructor for a error message clearer.
	  */
	virtual ~AMPIC887ErrorMessageClearer() {}

	/*!
	  * Starts the delay timer. The error message for the controller whose id the
	  * instance was initialized with will be cleared no earlier than the specified
	  * delay.
	  * \param mSecsDelay ~ The length of time (in milliseconds) after which the
	  * error message of the controller will be cleared.
	  */
	void start(int mSecsDelay);

protected:
	/*!
	  * The event function called when the delay timer has expired and the Qt event
	  * queue has processed the event. Clears the error message from the controller
	  * then schedules this instance for deletion.
	  */
	void timerEvent(QTimerEvent *);

	int controllerId_;
	int mSecsDelay_;
	int timerId_;

};

#endif // AMPIC887ERRORMESSAGECLEARER_H
