#include <QObject>

#include <QStringList>

class QProcess;

class AMDeploy : public QObject
{
Q_OBJECT
public:
	AMDeploy(const QString &projectFile, const QString &workingDirectory, QObject *parent = 0);
	virtual ~AMDeploy();

protected slots:
	void startDomainNameProcess();
	void onDomainNameProcessReadReady();
	void onDomainNameProcesFinished(int status);

	void startGitStatusProcess();
	void onGitStatusProcessReadReady();
	void onGitStatusProcessFinished(int status);

	void startGitBranchProcess();
	void onGitBranchProcessReadReady();
	void onGitBranchProcessFinished(int status);

	void startGitLogProcess();
	void onGitLogProcessReadReady();
	void onGitLogProcessFinished(int status);

	void startGitDescribeProcess();
	void onGitDescribeProcessReadReady();
	void onGitDescribeProcessFinished(int status);

	void startQMakeVersionProcess();
	void onQMakeVersionProcessReadReady();
	void onQMakeVersionProcessFinished(int status);

	void startCleanupObjectFilesProcess();
	void onCleanupObjectFilesProcessReadReady();
	void onCleanupObjectFilesProcessFinished(int status);

	void startQMakeProcess();
	void onQMakeProcessReadReady();
	void onQMakeProcessFinished(int status);

	void startMakeProcess();
	void onMakeProcessReadReady();
	void onMakeProcessFinished(int status);

protected:
	QString workingDirectory_;
	QString projectFile_;
	QStringList applicationArguments_;

	QProcess *domainNameProcess_;
	QString domainNameOutput_;

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

	QProcess *qmakeVersionProcess_;
	QString qmakeVersionOutput_;

	QProcess *cleanupObjectFilesProcess_;

	QProcess *qmakeProcess_;

	QProcess *makeProcess_;
};
