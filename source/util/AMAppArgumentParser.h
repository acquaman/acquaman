/*
	The parser to parse the customized application arguments
	Command-line arguments could be:
		* program argument: the argument of the program, no prefix and separated by space
		* single-character argument: starts with hyphen-minus "-" and separated by space
		* combined single-character argument
		* long/customized argument: prefixed with double hyphen-minuses "--"
		* can have no more than one argument value (separated with "=" or ":")

		eg: valid argument: -f, --force, -r, --recursive, -fr (equivalent to -f -r), -force (equivalent to -f -o -r -c -e)

	argument value:
		* the value of an argument, separated with special character "=" or ":".
		* combined single-character arguments can't have value

		eg: programName programArgument1 programArgument2, or
			programName -f programArgument1 -r programArgument2, or
			programname -f:argumentValue programArgument1 --load=argumentValue

	USAGES:
		AMAppArgumentParser *argumentParser = new AMAppArgumentParser(QApplication::instance()->arguments());


		bool hasArgument = argumentParser->hasArgument("--load");
		QString argumentValue = argumentParser->getArgumentValue("-l");
		int argumentType = argumentParser->getArgumentType("--load"); // will return 2 -- SINGLE_CHARACTER_ARGUMENT
		QStringList arguments = argumentParser->getArgumentsByType(AMAppArgumentParser::PROGRAM_ARGUMENT);
								 // will return a list of arguments with type 1 -- PROGRAM_ARGUMENT
*/

#ifndef AMAPPARGUMENTPARSER_H
#define AMAPPARGUMENTPARSER_H

#include <QStringList>
#include <QMap>
#include <QDebug>

class AMAppArgumentParser
{
public:
	static const QString ID_SINGLE_CHARACTER_ARGUMENT;
	static const QString ID_LONG_ARGUMENT;
	static const QString REGULAR_EXP_ARGUMENT_VALUE;

	enum AMAppArgumentType
	{
		PROGRAM_ARGUMENT = 1,
		SINGLE_CHARACTER_ARGUMENT = 2,
		LONG_ARGUMENT = 3,

		NOT_EXIST = 255
	};

	/// Constructor
	AMAppArgumentParser(QStringList arguments) {
		parseArguments(arguments);
	}

	/// Destructor
	virtual ~AMAppArgumentParser() { argumentValuePairs_.clear(); }

	/// check whether an argument is provided
	bool hasArgument(QString argument);

	/// get the type of a given argument
	int getArgumentType(QString argument);

	/// get the value of an argument
	/// return "" if there is no value or the argument doesn't exist
	QString getArgumentValue(QString argument);

	/// get a list of arguments in a given style/type
	QStringList getArgumentsByType(AMAppArgumentType argumentType);

	/// debugging <<< print the arguments
	void printArgumentPairs();

private:
	QString applicationName_;
	QMap<QString, QString> argumentValuePairs_;

	/// parse the argument list and store the argument/value pair to argumentValuePairs_
	void parseArguments(QStringList arguments);
};

#endif // AMAPPARGUMENTPARSER_H
