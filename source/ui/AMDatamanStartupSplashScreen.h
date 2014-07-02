/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").
Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AMDATAMANSTARTUPSPLASHSCREEN_H
#define AMDATAMANSTARTUPSPLASHSCREEN_H

#include <QWidget>
#include "util/AMErrorMonitor.h"

class QLabel;
class QProgressBar;
class QVBoxLayout;
class QHBoxLayout;

class AMDatamanStartupSplashScreen : public QWidget
{
Q_OBJECT

public:
	enum splashScreenMode{
		noMode = 0,
		waitingMode = 1
	};

 	virtual ~AMDatamanStartupSplashScreen();
	AMDatamanStartupSplashScreen(QWidget *parent = 0);

	int numberOfStages() const;

public slots:
	void onErrorMonStartupCode(AMErrorReport e);
	void onErrorMonStartupFinished(AMErrorReport e);

	void setNumberOfStages(int numberOfStages);

protected slots:
	void onErrorMonDebug(AMErrorReport e);
	void onErrorMonChangeMode(AMErrorReport e);

protected:
	QLabel *startupTextLabel_;
	QProgressBar *startupProgressBar_;
	QLabel *subStepTextLabel_;
	QProgressBar *waitingProgressBar_;

	QVBoxLayout *mainVL_;
	QHBoxLayout *subTextHL_;

	int numberOfStages_;
	int currentStage_;

	AMDatamanStartupSplashScreen::splashScreenMode specialMode_;
};

#endif // AMDATAMANSTARTUPSPLASHSCREEN_H
