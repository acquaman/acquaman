#include "AMListActionInfo3.h"

AMListActionInfo3::AMListActionInfo3(QObject *parent) :
	AMActionInfo3("List", "List of Actions to Run", ":/32x32/media-playlist-repeat.png", parent)
{
}

AMListActionInfo3::AMListActionInfo3(const AMListActionInfo3 &other) :
	AMActionInfo3(other)
{
}

AMActionInfo3* AMListActionInfo3::createCopy() const{
	return new AMListActionInfo3(*this);
}

QString AMListActionInfo3::typeDescription() const{
	return "List";
}
