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


#ifndef SHUTTERTOOLMAINWINDOW_H
#define SHUTTERTOOLMAINWINDOW_H

#include <QWidget>
#include <QCloseEvent>
#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>

#include "actions3/AMAction3.h"
#include "beamline/BioXAS/BioXASShutterToolController.h"

class ShutterToolMainWindow : public QWidget
{
    Q_OBJECT

public:
	ShutterToolMainWindow(QWidget *parent = 0);
	~ShutterToolMainWindow();

signals:
	void closed();

protected slots:
    void onEnableButtonClicked();
    void onDisableButtonClicked();
    void onOpenButtonClicked();
    void onCloseButtonClicked();

    void onModelOpened();
    void onModelClosed();
    void onModelBetween();

    void enableAutomaticShuttersOpenGUI(bool enabled);
    void enableShutterControlGUI(bool enabled);

    void onInjection();
    void onPostInjection();

protected:
	BioXASShutterToolController* model();

	void closeEvent(QCloseEvent *event) {
		emit closed();
		event->accept();
	}

private:
    void createComponents();
    void makeConnections();
    void defaultSettings();

    QWidget* createAppStatusDisplay();
    QWidget* createShutterDisplay();
    void turnAutomaticShutterControlOn();
    void turnAutomaticShutterControlOff();

private:
	BioXASShutterToolController *model_;
    QVBoxLayout *mainLayout_;

    QLabel *appStatusLabel_;
    QPushButton *enableAutomaticShuttersOpenButton_;
    QPushButton *disableAutomaticShuttersOpenButton_;

    QLabel *shuttersStatusLabel_;
    QPushButton *openShuttersButton_;
    QPushButton *closeShuttersButton_;
};

#endif // SHUTTERTOOLMAINWINDOW_H
