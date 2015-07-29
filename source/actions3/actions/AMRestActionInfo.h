#ifndef AMRESTACTIONINFO_H
#define AMRESTACTIONINFO_H

#include "actions3/AMActionInfo3.h"

typedef QMap<QByteArray, QByteArray> AMRestActionRawHeaderMap;

class AMRestActionInfo : public AMActionInfo3
{
Q_OBJECT

Q_PROPERTY(QString urlString READ urlString WRITE setUrlString)
Q_PROPERTY(AMRestActionInfo::VerbRequestType verbRequestType READ verbRequestType WRITE setVerbRequestType)
Q_PROPERTY(AMRestActionInfo::ContentType contentType READ contentType WRITE setContentType)
Q_PROPERTY(AMRestActionRawHeaderMap rawHeaders READ rawHeaders WRITE setRawHeaders)
Q_PROPERTY(QByteArray requestData READ requestData WRITE setRequestData)

public:
	/// Enum for HTTP Verb request type
	enum VerbRequestType { GetRequest, PostRequest, PatchRequest, PutRequest, DeleteRequest };
	/// Enum for the HTTP Content type to use
	enum ContentType { JSONContent, FormURLEncoded };

	Q_INVOKABLE AMRestActionInfo(const QString &urlString, AMRestActionInfo::VerbRequestType verbRequestType, AMRestActionInfo::ContentType contentType = AMRestActionInfo::JSONContent, const QByteArray &requestData = QByteArray(), QObject *parent = 0);
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

	/// Returns the type of HTTP Content to be encoded as
	AMRestActionInfo::ContentType contentType() const { return contentType_; }

	/// Returns the map of all raw headers
	AMRestActionRawHeaderMap rawHeaders() const { return rawHeaders_; }

	/// Returns the request data
	QByteArray requestData() const { return requestData_; }

public slots:
	/// Sets the URL, including any URL options
	void setUrlString(const QString &urlString);

	/// Sets the type of HTTP Verb to be used
	void setVerbRequestType(AMRestActionInfo::VerbRequestType verbRequestType);

	/// Sets the type of HTTP Content to be used for encoding
	void setContentType(AMRestActionInfo::ContentType contentType);

	/// Sets the map of all raw headers
	void setRawHeaders(AMRestActionRawHeaderMap rawHeaders);

	/// Clears all raw headers
	void clearRawHeaders();
	/// Sets or resets the header value associated with the header name
	void setRawHeader(const QByteArray headerName, const QByteArray headerValue);

	/// Sets the request data
	void setRequestData(const QByteArray &requestData);

protected:
	/// URL for the REST API Call
	QString urlString_;

	/// The type of HTTP Verb to be used
	AMRestActionInfo::VerbRequestType verbRequestType_;

	/// The type of HTTP content we're going to encode as (and set in our header field)
	AMRestActionInfo::ContentType contentType_;

	/// The map of all raw headers
	AMRestActionRawHeaderMap rawHeaders_;

	/// The request data
	QByteArray requestData_;
};

Q_DECLARE_METATYPE(AMRestActionInfo::VerbRequestType)
Q_DECLARE_METATYPE(AMRestActionInfo::ContentType)
Q_DECLARE_METATYPE(AMRestActionRawHeaderMap)

#endif // AMRESTACTIONINFO_H
