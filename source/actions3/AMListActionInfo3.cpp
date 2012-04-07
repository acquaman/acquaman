#include "AMListActionInfo3.h"

// AMListActionInfo
////////////////////////////////

AMListActionInfo3::AMListActionInfo3(const QString &shortDescription, const QString &longDescription, const QString &iconFileName, QObject *parent)
	: AMActionInfo3(shortDescription, longDescription, iconFileName, parent)
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

// AMSequentialListActionInfo
////////////////////////////////

AMSequentialListActionInfo3::AMSequentialListActionInfo3(const QString &shortDescription, const QString &longDescription, const QString &iconFileName, QObject *parent)
	: AMListActionInfo3(shortDescription, longDescription, iconFileName, parent)
{
}

AMSequentialListActionInfo3::AMSequentialListActionInfo3(const AMSequentialListActionInfo3 &other) :
	AMListActionInfo3(other)
{
}

AMActionInfo3* AMSequentialListActionInfo3::createCopy() const{
	return new AMSequentialListActionInfo3(*this);
}

QString AMSequentialListActionInfo3::typeDescription() const{
	return "Sequential List";
}

// AMParallelListActionInfo
////////////////////////////////

AMParallelListActionInfo3::AMParallelListActionInfo3(const QString &shortDescription, const QString &longDescription, const QString &iconFileName, QObject *parent)
	: AMListActionInfo3(shortDescription, longDescription, iconFileName, parent)
{
}

AMParallelListActionInfo3::AMParallelListActionInfo3(const AMParallelListActionInfo3 &other) :
	AMListActionInfo3(other)
{
}

AMActionInfo3* AMParallelListActionInfo3::createCopy() const{
	return new AMParallelListActionInfo3(*this);
}

QString AMParallelListActionInfo3::typeDescription() const{
	return "Parallel List";
}
