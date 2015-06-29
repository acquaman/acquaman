#include "AMGCS2CompositeCommand.h"

AMGCS2CompositeCommand::AMGCS2CompositeCommand()
{
}

AMGCS2CompositeCommand::~AMGCS2CompositeCommand()
{
	foreach (AMGCS2Command* command, commandList_) {
		delete command;
	}
}

void AMGCS2CompositeCommand::addCommand(AMGCS2Command *command)
{
	commandList_.append(command);
}

QString AMGCS2CompositeCommand::outputString() const
{
	if(!wasSuccessful_) {
		return "";
	} else {
		return commandOutputs_;
	}
}

bool AMGCS2CompositeCommand::validateArguments()
{
	if(commandList_.isEmpty()) {
		lastError_ = "No commands to run.";
		return false;
	}

	return true;
}

bool AMGCS2CompositeCommand::runImplementation()
{
	for(int iCommand = 0, commandCount = commandList_.count();
		iCommand < commandCount;
		++iCommand) {

		AMGCS2Command* currentSubCommand = commandList_.at(iCommand);

		currentSubCommand->setControllerId(controllerId_);
		currentSubCommand->run();

		if(!currentSubCommand->wasSuccessful()) {
			lastError_ = QString("Command number %1 failed with message %2")
					.arg(iCommand + 1)
					.arg(currentSubCommand->lastError());

			return false;
		}

		QString commandOutput = currentSubCommand->outputString();

		if(!commandOutput.isEmpty()) {
			commandOutputs_.append(commandOutput);
			commandOutputs_.append("\n");
		}
	}

	commandOutputs_ = commandOutputs_.trimmed();
	return true;
}


