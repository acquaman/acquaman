#include "AMConsoleInputHandler.h"
#include "AMAbstractConsoleCommandParser.h"

AMConsoleInputHandler::AMConsoleInputHandler(AMAbstractConsoleCommandParser* commandParser,
										   QObject* parent)
	:QObject(parent)
{
	standardOutputStream_ = new QTextStream(stdout);
	standardErrorStream_ = new QTextStream(stderr);
	inputStream_ = new QTextStream(stdin);
	inputStreamNotifier_ = new QSocketNotifier(fileno(stdin), QSocketNotifier::Read, this);
	commandParser_ = commandParser;

	commandPrompt_ = "> ";
	commandParser_->setParent(this);
	connect(inputStreamNotifier_, SIGNAL(activated(int)), this, SLOT(onInputReceived()));
	connect(commandParser_, SIGNAL(unknownCommand(QString)), this, SLOT(onCommandUnknown(QString)));
	connect(commandParser_, SIGNAL(parseComplete()), this, SLOT(displayCommandPrompt()));
}

AMConsoleInputHandler::~AMConsoleInputHandler()
{
	delete standardOutputStream_;
	delete standardErrorStream_;
	delete inputStream_;
}

void AMConsoleInputHandler::writeLineToStandardOutput(const QString &output)
{
	(*standardOutputStream_) << output << endl;
}

void AMConsoleInputHandler::writeLineToStandardError(const QString &output)
{
	(*standardErrorStream_) << output << endl;
}

void AMConsoleInputHandler::writeToStandardOutput(const QString &output)
{
	(*standardOutputStream_) << output;
	standardOutputStream_->flush();
}

void AMConsoleInputHandler::writeToStandardError(const QString &output)
{
	(*standardErrorStream_) << output;
	standardErrorStream_->flush();
}

void AMConsoleInputHandler::setCommandPrompt(const QString &commandPrompt)
{
	commandPrompt_ = commandPrompt;
}

void AMConsoleInputHandler::displayCommandPrompt()
{
	writeToStandardOutput(commandPrompt_);
}

void AMConsoleInputHandler::onInputReceived()
{
	QString receivedInput = inputStream_->readLine();
	commandParser_->interpretCommand(receivedInput);
}

void AMConsoleInputHandler::onCommandUnknown(const QString &errorMessage)
{
	writeLineToStandardOutput(errorMessage);
}



