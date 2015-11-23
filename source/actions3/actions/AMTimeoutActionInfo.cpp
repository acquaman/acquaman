#include "AMTimeoutActionInfo.h"

AMTimeoutActionInfo::AMTimeoutActionInfo(double waitTime, const QString &instruction, QObject *parent) :
	AMWaitActionInfo(waitTime, parent)
{
	instruction_ = instruction;

	setShortDescription(typeDescription());
	setLongDescription(typeDescription());
}

AMTimeoutActionInfo::AMTimeoutActionInfo(const AMTimeoutActionInfo &original) :
	AMWaitActionInfo(original)
{
	instruction_ = original.instruction();
}

AMTimeoutActionInfo::~AMTimeoutActionInfo()
{

}

AMActionInfo3* AMTimeoutActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMTimeoutActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}

void AMTimeoutActionInfo::setInstruction(const QString &newInstruction)
{
	if (instruction_ != newInstruction) {
		instruction_ = newInstruction;
		setModified(true);

		emit instructionChanged(instruction_);
	}
}
