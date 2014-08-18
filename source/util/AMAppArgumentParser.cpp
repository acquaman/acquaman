#include "AMAppArgumentParser.h"

#include <QStringList>
#include <QDebug>

//===================== public member functions =======================
bool AMAppArgumentParser::hasArgument(QString argument)
{
	return argumentValuePairs_.contains(argument);
}

int AMAppArgumentParser::argumentType(QString argument)
{
	int type = AMAppArgumentParser::NotExist;

	if (hasArgument(argument)) {
		if (argument.startsWith(idSingleCharacterArgument_))
			type = AMAppArgumentParser::LongArgument;
		else if (argument.startsWith(idLongArgument_))
			type = AMAppArgumentParser::SingleCharacterArgument;
		else
			type = AMAppArgumentParser::ProgramArgument;
	}

	return type;
}

QStringList AMAppArgumentParser::argumentsByType(AMAppArgumentType type) {
	QStringList arguments;

	QStringList keys = argumentValuePairs_.keys();
	for (int i = 0; i < keys.length(); i++) {
		if (argumentType(keys.at(i)) == type) {
			arguments.append(keys.at(i));
		}
	}

	return arguments;
}

QString AMAppArgumentParser::argumentValue(QString argument)
{
	return hasArgument(argument) ? argumentValuePairs_.value(argument) : NULL;
}

// debugging usage
void AMAppArgumentParser::printArgumentPairs()
{
	qDebug() << "Program: " << applicationName_;
	qDebug() << "argument" << "	" << "value";
	qDebug() << "----------------------------------------";

	for (int i = 0; i < argumentValuePairs_.count(); i++) {
		QString key = argumentValuePairs_.keys().at(i);
		qDebug()<< key << "	" << argumentValuePairs_.value(key);
	}
}


//===================== private member functions =======================

void AMAppArgumentParser::parseArguments(QStringList arguments)
{
	int cursor = 0;

	applicationName_ = arguments.at(cursor++);

	//handle the program argument
	while (cursor < arguments.size()) {
		QString argument = arguments.at(cursor++);

		QStringList listArgumentValue = argument.split(QRegExp(regularExpressionOfArgumentValue_));
		QString argumentName = listArgumentValue.at(0);
		QString value = listArgumentValue.length() > 1 ? listArgumentValue.at(1) : NULL;

		if (argumentName.startsWith(idSingleCharacterArgument_)) {

			// if it is a long option or a short option
			if (argumentName.startsWith(idLongArgument_) || argumentName.length() == 2) {
				argumentValuePairs_.insert(argumentName, value);

			} else if (value.length() == 0) {
				for (int i = 1; i < argumentName.length(); i++) {
					argumentValuePairs_.insert(QString(idSingleCharacterArgument_).append(argumentName.at(i)), NULL);
				}
			} else {
				// ERROR: This should never happen: combined arguments with value
				qDebug() << "Invalid format: combined arguments with values. Argument: " << argument;
			}

		} else {
			argumentValuePairs_.insert(argumentName, value);
		}
	}
}
