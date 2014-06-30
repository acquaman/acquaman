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


#include <QApplication>
#include <QSettings>
#include "ui/beamline/camera/AMSampleCameraBrowserView.h"

/// Helper function to load application settings using QSettings, and set the camera browser defaults (Crosshair settings,
void loadSettings(AMSampleCameraBrowserView* camBrowser);

/// Helper function to save application settings using QSettings, based on the camera browser's current state
void saveSettings(AMSampleCameraBrowserView* camBrowser);


int main(int argc, char *argv[])
{

	/// Program Startup:
	// =================================
	QApplication app(argc, argv);
	app.setApplicationName("AcquaCam");

	QStringList arguments = app.arguments();
	bool disableOpenGl = arguments.contains("--disable-opengl") || arguments.contains("-g");

	AMSampleCameraBrowserView* camBrowser = new AMSampleCameraBrowserView(0, !disableOpenGl);
	camBrowser->setWindowTitle("AcquaCam");
	camBrowser->show();

	loadSettings(camBrowser);

	/// Program Run-loop:
	// =================================
	int retVal = app.exec();

	/// Program Shutdown:
	// =================================
	saveSettings(camBrowser);
	delete camBrowser;

	return retVal;
}


void loadSettings(AMSampleCameraBrowserView* camBrowser) {
	QSettings settings(QSettings::UserScope, "Acquaman", "AcquaCam");

	QColor crosshairColor = settings.value("Crosshair/Color", QColor(Qt::red)).value<QColor>();
	int crosshairThickness = settings.value("Crosshair/Thickness", 1).toInt();
	bool crosshairVisible = settings.value("Crosshair/Visible", true).toBool();
	bool crosshairLocked = settings.value("Crosshair/Locked", false).toBool();
	QPointF crosshairPosition = settings.value("Crosshair/Position", QPointF(0.5,0.5)).toPointF();
	QStringList previousSources = settings.value("Sources/Previous", QStringList()).toStringList();
	QString currentSource = settings.value("Sources/Current", QString()).toString();

	camBrowser->setCrosshairColor(crosshairColor);
	camBrowser->setCrosshairLineThickness(crosshairThickness);
	camBrowser->setCrosshairVisible(crosshairVisible);
	camBrowser->setCrosshairLocked(crosshairLocked);
	camBrowser->setCrosshairPosition(crosshairPosition);

	camBrowser->setCurrentSourceURL(currentSource);
	camBrowser->setPreviousSourceURLs(previousSources);
}

void saveSettings(AMSampleCameraBrowserView *camBrowser) {
	QSettings settings(QSettings::UserScope, "Acquaman", "AcquaCam");

	settings.setValue("Crosshair/Color", camBrowser->crosshairColor());
	settings.setValue("Crosshair/Thickness", camBrowser->crosshairLineThickness());
	settings.setValue("Crosshair/Visible", camBrowser->crosshairVisible());
	settings.setValue("Crosshair/Locked", camBrowser->crosshairLocked());
	settings.setValue("Crosshair/Position", camBrowser->crosshairPosition());
	settings.setValue("Sources/Previous", camBrowser->previousSourceURLs());
	settings.setValue("Sources/Current", camBrowser->currentSourceURL());
}
