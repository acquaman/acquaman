#include "AMRestActionInfo.h"

AMRestActionInfo::AMRestActionInfo(const QString &urlString, AMRestActionInfo::VerbRequestType verbRequestType, QObject *parent)
	: AMActionInfo3("REST API Call", "REST API Call", ":/user-away.png", parent)
{
	urlString_ = urlString;
	verbRequestType_ = verbRequestType;

	setShortDescription("REST API Call");
	setLongDescription("REST API Call");
}

AMRestActionInfo::AMRestActionInfo(const AMRestActionInfo &other)
	: AMActionInfo3(other)
{
	urlString_ = other.urlString();
	verbRequestType_ = other.verbRequestType();
}

AMActionInfo3 *AMRestActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMRestActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}

void AMRestActionInfo::setUrlString(const QString &urlString)
{
	urlString_ = urlString;
}

void AMRestActionInfo::setVerbRequestType(AMRestActionInfo::VerbRequestType verbRequestType)
{
	verbRequestType_ = verbRequestType;
}

void AMRestActionInfo::setRawHeaders(AMRestActionRawHeaderMap rawHeaders)
{
	rawHeaders_ = rawHeaders;
}

void AMRestActionInfo::clearRawHeaders()
{
	rawHeaders_.clear();
}

void AMRestActionInfo::setRawHeader(const QByteArray headerName, const QByteArray headerValue)
{
	rawHeaders_.insert(headerName, headerValue);
}
