#ifndef AMBUILDINFO_H
#define AMBUILDINFO_H

#include <QString>

class AMBuildInfo
{

public:
	AMBuildInfo();

	QString commitSHA() const;
	QString branchName() const;
	QString tagName() const;
	QString describeName() const;
	QString lastCommitAuthor() const;

protected:
	void setCommitSHA(const QString &commitSHA);
	void setBranchName(const QString &branchName);
	void setTagName(const QString &tagName);
	void setDescribeName(const QString &describeName);
	void setLastCommitAuthor(const QString &lastCommitAuthor);

protected:
	QString commitSHA_;
	QString branchName_;
	QString tagName_;
	QString describeName_;
	QString lastCommitAuthor_;
};

#endif // AMBUILDINFO_H
