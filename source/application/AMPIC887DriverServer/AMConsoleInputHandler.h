#ifndef AMCONSOLEINPUTHANDLER_H
#define AMCONSOLEINPUTHANDLER_H

#include <QObject>
#include <QTextStream>
#include <QString>
#include <QSocketNotifier>

class AMAbstractConsoleCommandParser;
/*!
  * \brief A class which controls input from and output to a console terminal.
  *
  * Each console application expects to be passed an implementation of an abstract
  * command parser of which it will take ownership. The command parser will be used to
  * interpret the commands entered by the user in the console application, and then
  * emit signals based on the recognized user input.
  * See AMAbstractConsoleCommandParser for more information on the creation of a command
  * parser.
  * See AMHexapodDriverApplication for an example of an implementation of a full
  * console application.
  */
class AMConsoleInputHandler : public QObject {
	Q_OBJECT
public:
	/*!
	  * Creates an instance of a console application with the provided command
	  * set.
	  * \param commandParser ~ The console command parser which will be used to
	  * interpret the user input commands.
	  */
	explicit AMConsoleInputHandler(AMAbstractConsoleCommandParser* commandParser, QObject* parent = 0);

	/*!
	  * Base virtual destructor for a console application.
	  */
	virtual ~AMConsoleInputHandler();

	/*!
	  * Writes the provided output to standard output as a single line, then flushes
	  * the output buffer.
	  * \param output ~ The text which will be written to standard output.
	  */
	void writeLineToStandardOutput(const QString& output);

	/*!
	  * Writes the provided output to standard error as a single line, then flushes
	  * the output buffer.
	  * \param output ~ The text which will be written to standard error.
	  */
	void writeLineToStandardError(const QString& output);

	/*!
	  * Writes the provided outut to standard output and flushes the output buffer.
	  * No line break is inserted.
	  * \param output ~ The text which will be written to standard output.
	  */
	void writeToStandardOutput(const QString& output);

	/*!
	  * Writes the provided outut to standard error and flushes the output buffer.
	  * No line break is inserted.
	  * \param output ~ The text which will be written to standard error.
	  */
	void writeToStandardError(const QString& output);

	/*!
	  * Sets the text prompt which will displayed to users to indicate that the
	  * console is ready to receive input.
	  * \param commandPrompt ~ The text to display as a command prompt.
	  */
	void setCommandPrompt(const QString& commandPrompt);
public slots:
	/*!
	  * Displays the command prompt to the console to indicate that it is ready
	  * for input.
	  */
	void displayCommandPrompt();
protected slots:
	/*!
	  * Handles the input socket notifier signalling that there is input to be read
	  * from the standard input stream.
	  */
	void onInputReceived();
	/*!
	  * Handles the contained command set signalling that it does not recognize
	  * a command it has been asked to interpret.
	  * \param errorMessage ~ The response text from the command set indicating
	  * why it could not understand the command.
	  */
	void onCommandUnknown(const QString& errorMessage);
protected:
	/// A text stream for writing to standard output.
	QTextStream* standardOutputStream_;
	/// A text stream for writing to standard error.
	QTextStream* standardErrorStream_;
	/// A text stream for reading from standard input.
	QTextStream* inputStream_;
	/// A socket notifier which emits signals when there is input to be read from
	/// the standard input.
	QSocketNotifier* inputStreamNotifier_;
	/// The abstract command set which will be passed an input.
	AMAbstractConsoleCommandParser* commandParser_;
	/// The prompt to display to the console to indicate that it is ready for input
	QString commandPrompt_;
};

#endif // AMCONSOLEINPUTHANDLER_H
