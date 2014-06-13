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

protected:
	QProcess *gitStatusProcess_;
};
