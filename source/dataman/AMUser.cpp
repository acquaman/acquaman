#include "AMUser.h"

// Static instance variable
AMUser* AMUser::instance_ = 0;

// Protected default constructor
AMUser::AMUser() :
	AMDbObject()
{
	currentRunId_ = -1;
}

