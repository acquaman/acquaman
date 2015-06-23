#ifndef AMABSTRACTCONSOLECOMMANDPARSER_H
#define AMABSTRACTCONSOLECOMMANDPARSER_H

#include <QObject>

/*!
  * \brief An abstract class which represents an interface for receiving command
  * input then interpreting it.
  *
  * In order to implement this class it is required to provide a definition for the
  * interpretCommandImplementation function. It is recommended that these commands
  * are linked to signals which the subclass will emit, and can be listened to
  * by any class interested. For any command recevied which cannot be interpreted
  * it is recommended to emit the unknownCommand() signal.
  *
  * For Example:
  *  The subclass might contain a signal quit(), with the interpretCommandImplementation
  *  class then checking if (command == "\q") emit quit();
  *  An interested class (for example a subclass of a QCoreApplication can then
  *  listen for the command signals quit() signal, and invoke qApp->quit();
  *
  * For a full example of an implementation of a console application see
  * AMHexapodDriverApplication.
  */
class AMAbstractConsoleCommandParser : public QObject
{
	Q_OBJECT
public:
	/*!
	  * Default constructor for an abstract console command set.
	  */
	explicit AMAbstractConsoleCommandParser(QObject *parent = 0);

	/*!
	  * Default virtual destructor for an abstract console command set.
	  */
	virtual ~AMAbstractConsoleCommandParser() { }

	/*!
	  * Receives the command to be interpreted, then passes this to the subclass'
	  * specific interpretCommandImplementation.
	  * \param command ~ The text to be interpreted into a command.
	 */
	void interpretCommand(const QString& command);
signals:
	/*!
	  * Signal which indicates that the last received command was not known to the
	  * command set.
	  * \param errorMessage ~ Text detailing why the command was not interpreted.
	  */
	void unknownCommand(const QString& command);

	/*!
	  * Signal which indicates that parsing is complete, and the parser is ready
	  * to receive additional input.
	  */
	void parseComplete();
public slots:

protected:
	/*!
	  * Subclass specific implementation which interprets the provided command.
	  *
	  * In the base class this is a pure virtual function.
	  * \param command ~ The text to be interpreted into a command.
	  */
	virtual void interpretCommandImplementation(const QString& command) = 0;

};

#endif // AMABSTRACTCONSOLECOMMANDPARSER_H
