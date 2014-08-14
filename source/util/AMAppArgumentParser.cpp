#include "AMAppArgumentParser.h"

#include <QStringList>
#include <QDebug>

//===================== public member functions =======================
const QString AMAppArgumentParser::SINGLE_CHARACTER_ARGUMENT = "-";
const QString AMAppArgumentParser::LONG_ARGUMENT = "--";
const QString AMAppArgumentParser::REGULAR_EXP_ARGUMENT_VALUE = "\\=|:";

bool AMAppArgumentParser::hasArgument(QString argument)
{
	return argumentValuePairs_.contains(argument);
}

int AMAppArgumentParser::getArgumentType(QString argument)
{
	int type = NOT_EXIST;

	if (hasArgument(argument)) {
		if (argument.startsWith(SINGLE_CHARACTER_ARGUMENT))
			type = LONG_ARGUMENT;
		else if (argument.startsWith(LONG_ARGUMENT))
			type = SINGLE_CHARACTER_ARGUMENT;
		else
			type = PROGRAM_ARGUMENT;
	}

	return type;
}

QStringList AMAppArgumentParser::getArgumentsByType(AMAppArgumentType argumentType) {
	QStringList arguments;

	QStringList keys = argumentValuePairs_.keys();
	for (int i = 0; i < keys.length(); i++) {
		if (getArgumentType(keys.at(i)) == argumentType) {
			arguments.append(keys.at(i));
		}
	}

	return arguments;
}

QString AMAppArgumentParser::getArgumentValue(QString argument)
{
	QString value = NULL;

	if (argumentValuePairs_.contains(argument)) {
		value = argumentValuePairs_.value(argument);
	}

	return value;
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

		if (argument.startsWith(SINGLE_CHARACTER_ARGUMENT)) {
			QStringList listArgumentValue = argument.split(QRegExp(REGULAR_EXP_ARGUMENT_VALUE));
			QString argumentName = listArgumentValue.at(0);
			QString value = listArgumentValue.length() > 1 ? listArgumentValue.at(1) : NULL;

			// if it is a long option or a short option
			if (argumentName.startsWith(LONG_ARGUMENT) || argumentName.length() == 2) {
				argumentValuePairs_.insert(argumentName, value);

			} else if (value.length() == 0) {
				for (int i = 1; i < argumentName.length(); i++) {
					argumentValuePairs_.insert(QString("-").append(argumentName.at(i)), NULL);
				}
			} else {
				// ERROR: This should never happen: combined options with value
			}

		} else {
			argumentValuePairs_.insert(argument, NULL);
		}
	}
}
