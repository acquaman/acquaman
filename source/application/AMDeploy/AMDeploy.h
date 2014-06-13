#include <QObject>

class QProcess;

class AMDeploy : public QObject
{
Q_OBJECT
public:
	AMDeploy(QObject *parent = 0);
	virtual ~AMDeploy();

protected slots:
	void onGitStatusProcessReadReady();
	void onGitStatusProcessFinished(int status);

	void onGitBranchProcessReadReady();
	void onGitBranchProcessFinished(int status);

	void onGitLogProcessReadReady();
	void onGitLogProcessFinished(int status);

	void onGitDescribeProcessReadReady();
	void onGitDescribeProcessFinished(int status);

protected:
	QProcess *gitStatusProcess_;
	QString gitStatusOutput_;

	QProcess *gitBranchProcess_;
	QString gitBranchOutput_;

	QProcess *gitLogProcess_;
	QString gitLogOutput_;

	QProcess *gitDescribeProcess_;
	QString gitDescribeOutput_;

	QString currentBranch_;
	QString currentCommitSHA_;
	QString currentCommitAuthor_;
	QString currentCommitDate_;
	QString currentGitDescription_;
};
