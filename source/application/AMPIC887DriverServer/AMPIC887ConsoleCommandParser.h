#ifndef AMPIC887CONSOLECOMMANDPARSER_H
#define AMPIC887CONSOLECOMMANDPARSER_H

#include <QStringList>

#include "AMAbstractConsoleCommandParser.h"
#include "AMGCS2.h"
#include <QHash>
/*!
  * A class which represents the commands which a user can input to the Hexapod
  * C887 Controller driver server system.
  */
class AMGCS2Command;
class AMPIC887ConsoleCommandParser : public AMAbstractConsoleCommandParser
{
	Q_OBJECT
public:
	/*!
	  * Creates a new instance of hexapod driver console command set.
	  */
	AMPIC887ConsoleCommandParser(QObject* parent = 0);

	/*!
	  * Standard virtual destructor for a hexapod driver console command set.
	  */
	virtual ~AMPIC887ConsoleCommandParser() {}

	/*!
	  * Helper method which lists the current available commands.
	  */
	QString commandList();
signals:
	/*!
	  * Signal indicating that a get available parameters command has been issued.
	  */
	void availableParametersCommandIssued();

	/*!
	  * Signal indicating that a move command has been issued
	  */
	void moveCommandIssued(const QHash<AMGCS2::Axis, double>& targetPositions);

	/*!
	  * Signal indicating that a move relative command has been issued.
	  */
	void moveRelativeCommandIssued(const QHash<AMGCS2::Axis, double>& relativeTargetPositions);

	/*!
	  * Signal indicating that a reference move command has been issued.
	  */
	void referenceMoveCommandIssued(const QList<AMGCS2::Axis>& axes);

	/*!
	  * Signal indicating that a motion status command has been issued.
	  */
	void motionStatusCommandIssued();

	/*!
	  * Signals that the quit command has been issued.
	  */
	void quit();

	/*!
	  * Signals that the help command has been issued.
	  */
	void help();

	/*!
	  * Signals that the staus command has been issued.
	  */
	void status();

	/*!
	  * Signals that the command to switch to a different active controller has
	  * been issued.
	  * \param controllerName ~ The name of the controller to make the active
	  * controller.
	  */
	void changeActiveController(const QString& controllerName);

	/*!
	  * Signals that a command other than the standard ones handled by the parser
	  * has been issued.
	  */
	void otherCommandIssued(const QString& commandString);
protected:
	/*!
	  * Interprets the provided command using the list of acceptable commands which
	  * can be issued to the hexapod driver console.
	  * \param command ~ The command which was issued to the hexapod driver console.
	  */
	virtual void interpretCommandImplementation(const QString &command);

	/*!
	  * Helper method for obtaining a list of passed argument from a command string.
	  * \param commandString ~ The string from which the arguments are to be
	  * retrieved.
	  */
	QStringList commandArguments(const QString& commandString);

	/*!
	  * Helper method for converting a command arguments in string format into a
	  * list of Axes.
	  * \param axisArguments ~ The QString argument to be converted to a list of
	  * axes.
	  */
	QList<AMGCS2::Axis> axesFromCommandString(const QString& axisArguments);

	/*!
	  * Helper method for converting a command arguments into a mapping of axis
	  * to double value. Assumes that the arguments are provided as:
	  *    axis1 value1 axis2 value2 ... axisN valueN
	  * \param arguments ~ The QString argument to be converted to a mapping of
	  * axis to value.
	  */
	QHash<AMGCS2::Axis, double> axesDoublePairFromCommandString(const QString& arguments);
};

#endif // AMPIC887CONSOLECOMMANDSET_H
