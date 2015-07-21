#ifndef AMGITHUBGETCOMMENTSACTIONINFO_H
#define AMGITHUBGETCOMMENTSACTIONINFO_H

#include "actions3/AMActionInfo3.h"

class AMGitHubGetCommentsActionInfo : public AMActionInfo3
{
Q_OBJECT

Q_PROPERTY(QString owner READ owner WRITE setOwner)
Q_PROPERTY(QString repo READ repo WRITE setRepo)
Q_PROPERTY(QStringList commentURLs READ commentURLs WRITE setCommentURLs)

public:
	Q_INVOKABLE AMGitHubGetCommentsActionInfo(const QString &owner, const QString &repo, const QStringList &commentURLs, QObject *parent = 0);
	/// Copy Constructor.
	AMGitHubGetCommentsActionInfo(const AMGitHubGetCommentsActionInfo &other);
	/// Destructor.
	virtual ~AMGitHubGetCommentsActionInfo(){}

	/// This function is used as a virtual copy constructor.
	virtual AMActionInfo3 *createCopy() const;

	/// This should describe the type of the action.
	virtual QString typeDescription() const { return QString("REST call to retrieve comments from %1/%2").arg(owner_).arg(repo_); }

	/// Returns the owner of the github repo
	QString owner() const { return owner_; }

	/// Returns the name of the github repo
	QString repo() const { return repo_; }

	/// Returns the list of URLs we're going to iterate for comments
	QStringList commentURLs() const { return commentURLs_; }

public slots:
	/// Sets the owner of the repo
	void setOwner(const QString &owner);

	/// Sets the repo to search
	void setRepo(const QString &repo);

	/// Set the list of URLs we're going to iterate for comments
	void setCommentURLs(const QStringList &commentURLs);

protected:
	/// Name of the owner of the github repo
	QString owner_;

	/// Name of the github repo
	QString repo_;

	/// The list of URLs we're going to iterate for comments
	QStringList commentURLs_;
};

#endif // AMGITHUBGETCOMMENTSACTIONINFO_H
