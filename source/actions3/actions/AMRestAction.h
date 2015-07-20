#ifndef AMRESTACTION_H
#define AMRESTACTION_H

#include "actions3/AMAction3.h"
#include "actions3/actions/AMRestActionInfo.h"

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QDateTime>

#define AMRESTACTION_CANNOT_START_WITH_EMPTY_URL 355001
#define AMRESTACTION_CANNOT_START_WITHOUT_NETWORKACCESSMANAGER 355002
#define AMRESTACTION_NETWORK_ERROR_FAILURE 355003

class AMRestAction : public AMAction3
{
Q_OBJECT
public:
	/// Constructor.
	Q_INVOKABLE AMRestAction(AMRestActionInfo *info, QNetworkAccessManager *networkAccessManager,  QObject *parent = 0);
	/// Copy constructor.
	AMRestAction(const AMRestAction &other);
	/// Destructor.
	virtual ~AMRestAction() {}

	/// Virtual copy constructor.
	virtual AMAction3 *createCopy() const;

	/// Specify that we can pause.
	virtual bool canPause() const { return false; }
	/// Specify that we can skip.
	virtual bool canSkip() const { return false; }

	/// Virtual function that denotes that this action has no children.
	virtual bool hasChildren() const { return false; }
	/// Virtual function that returns the number of children for this action.
	virtual int numberOfChildren() const { return 0; }

	/// We can always access our info object via info_ or info(), but it will come back as a AMActionInfo* pointer that we would need to cast to AMRestActionInfo. This makes it easier to access.
	const AMRestActionInfo *restActionInfo() const { return qobject_cast<const AMRestActionInfo *>(info()); }
	/// We can always access our info object via info_ or info(), but it will come back as a AMActionInfo* pointer that we would need to cast to AMRestActionInfo. This makes it easier to access.
	AMRestActionInfo* restActionInfo() { return qobject_cast<AMRestActionInfo *>(info()); }

	QNetworkAccessManager* networkAccessManager() const { return networkAccessManager_; }

	/// Returns a string in a readable format based on the returned values when requesting the issues from the respository.
	QString jsonSensiblePrint(const QVariantMap &jsonMap, int indentLevel = 0) const;

	static int pageNumberFromURLString(const QString &urlString);

	/// Returns the generic JSON response
	QVariant fullResponse() const { fullResponse_;}

public slots:
	/// Sets the pointer to a long-lived network access manager. This action will not clean up the memory or take ownership of this.
	void setNetworkAccessManager(QNetworkAccessManager *networkAccessManager);

signals:
	void fullResponseReady(QVariant fullResponse, QList<QNetworkReply::RawHeaderPair>);

protected:
	/// This function is called from the Starting state when the implementation should initiate the action.
	virtual void startImplementation();
	/// Pauses the action by stopping the timer.
	virtual void pauseImplementation();
	/// Resumes the action by resuming the timer.
	virtual void resumeImplementation();
	/// Handles cancelling the scan.
	virtual void cancelImplementation();
	/// Handles skipping the action.
	virtual void skipImplementation(const QString &command);


protected slots:
	void onNetworkRequestReturned();
	void onNetworkRequestError(QNetworkReply::NetworkError networkError);

protected:
	/// Network reply for our request
	QNetworkReply *restReply_;
	/// Pointer to a long-lived network access manager
	QNetworkAccessManager *networkAccessManager_;

	/// The generic JSON response
	QVariant fullResponse_;
};

#endif // AMRESTACTION_H
