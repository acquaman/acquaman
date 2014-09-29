#ifndef MOTORTOOLMAINSCREEN_H
#define MOTORTOOLMAINSCREEN_H

#include <QTabWidget>
#include <QCloseEvent>

#include "beamline/BioXAS/BioXASToolSuite/BioXASBeamlineDataModel.h"

class MotorToolMainScreen : public QTabWidget
{
    Q_OBJECT

public:
    explicit MotorToolMainScreen(QWidget *parent = 0);
    ~MotorToolMainScreen();

signals:
	void closed();

protected:
	void closeEvent(QCloseEvent *event) {
		emit closed();
		event->accept();
	}

private:
	void setupUi();
};

#endif // MOTORTOOLMAINSCREEN_H
