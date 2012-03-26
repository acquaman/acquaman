#ifndef AMDATAMANSTARTUPSPLASHSCREEN_H
#define AMDATAMANSTARTUPSPLASHSCREEN_H

#include <QWidget>
#include "util/AMErrorMonitor.h"

class QLabel;
class QProgressBar;

class AMDatamanStartupSplashScreen : public QWidget
{
Q_OBJECT

public:
	AMDatamanStartupSplashScreen(QWidget *parent = 0);

public slots:
	void onErrorMonStartupCode(AMErrorReport e);
	void onErrorMonStartupFinished(AMErrorReport e);

	void setNumberOfStages(int numberOfStages);

protected slots:
	void onErrorMonDebug(AMErrorReport e);

protected:
	QLabel *startupTextLabel_;
	QProgressBar *startupProgressBar_;
	QLabel *subStepTextLabel_;

	int numberOfStages_;
	int currentStage_;
};

#endif // AMDATAMANSTARTUPSPLASHSCREEN_H
