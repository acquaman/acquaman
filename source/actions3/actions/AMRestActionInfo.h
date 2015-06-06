#ifndef AMRESTACTIONINFO_H
#define AMRESTACTIONINFO_H

#include "actions3/AMActionInfo3.h"

typedef QMap<QByteArray, QByteArray> AMRestActionRawHeaderMap;

class AMRestActionInfo : public AMActionInfo3
{
Q_OBJECT

Q_PROPERTY(QString urlString READ urlString WRITE setUrlString)
Q_PROPERTY(AMRestActionInfo::VerbRequestType verbRequestType READ verbRequestType WRITE setVerbRequestType)
Q_PROPERTY(AMRestActionRawHeaderMap rawHeaders READ rawHeaders WRITE setRawHeaders)

public:
	/// Enum for HTTP Verb request type
	enum VerbRequestType { GetRequest, PostRequest, PatchRequest };

	Q_INVOKABLE AMRestActionInfo(const QString &urlString, AMRestActionInfo::VerbRequestType verbRequestType, QObject *parent = 0);
	/// Copy Constructor.
	AMRestActionInfo(const AMRestActionInfo &other);
	/// Destructor.
	virtual ~AMRestActionInfo(){}

	/// This function is used as a virtual copy constructor.
	virtual AMActionInfo3 *createCopy() const;

	/// This should describe the type of the action.
	virtual QString typeDescription() const { return QString("REST API call to %1").arg(urlString_); }

	/// Returns the URL for the REST API Call
	QString urlString() const { return urlString_; }

	/// Returns the type of HTTP Verb to be used
	AMRestActionInfo::VerbRequestType verbRequestType() const { return verbRequestType_; }

	/// Returns the map of all raw headers
	AMRestActionRawHeaderMap rawHeaders() const { return rawHeaders_; }

public slots:
	/// Sets the URL, including any URL options
	void setUrlString(const QString &urlString);

	/// Sets the type of HTTP Verb to be used
	void setVerbRequestType(AMRestActionInfo::VerbRequestType verbRequestType);

	/// Sets the map of all raw headers
	void setRawHeaders(AMRestActionRawHeaderMap rawHeaders);

	/// Clears all raw headers
	void clearRawHeaders();
	/// Sets or resets the header value associated with the header name
	void setRawHeader(const QByteArray headerName, const QByteArray headerValue);

protected:
	/// URL for the REST API Call
	QString urlString_;

	/// The type of HTTP Verb to be used
	AMRestActionInfo::VerbRequestType verbRequestType_;

	/// The map of all raw headers
	AMRestActionRawHeaderMap rawHeaders_;
};

Q_DECLARE_METATYPE(AMRestActionInfo::VerbRequestType)
Q_DECLARE_METATYPE(AMRestActionRawHeaderMap)

#endif // AMRESTACTIONINFO_H
