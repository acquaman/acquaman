#include "AMAxisValueFinishedActionInfo.h"

AMAxisValueFinishedActionInfo::AMAxisValueFinishedActionInfo(QObject *parent)
	: AMActionInfo3("Axis Value Finished", "Axis Value Finished", ":/system-run.png", parent)
{
}

AMAxisValueFinishedActionInfo::AMAxisValueFinishedActionInfo(const AMAxisValueFinishedActionInfo &other)
	: AMActionInfo3(other)
{

}
