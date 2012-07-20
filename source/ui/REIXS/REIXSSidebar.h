/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef REIXSSIDEBAR_H
#define REIXSSIDEBAR_H

#include <QWidget>

namespace Ui {
    class REIXSSidebar;
}

class REIXSActionBasedControlEditor;

/// This widget displays the real-time "at a glance" REIXS beamline controls / feedback monitors on the side of the application's main window.
class REIXSSidebar : public QWidget
{
    Q_OBJECT

public:
    explicit REIXSSidebar(QWidget *parent = 0);
    ~REIXSSidebar();

private:
    Ui::REIXSSidebar *ui;

	// Additional UI controls
	REIXSActionBasedControlEditor* beamlineEnergyEditor_, *monoSlitEditor_, *gratingSelector_, *mirrorSelector_;

protected slots:
	// Responding to GUI events
	////////////////////////////

	void onBeamOnButtonClicked();
	void onBeamOffButtonClicked();

	// Responding to beamline events:
	///////////////////////////////////

	/// This could be a little sub-widget:
	void onMCPCountsPerSecondChanged(double countsPerSecond);
	void onTEYCountsChanged(double counts);
	void onTFYCountsChanged(double counts);
	void onI0CountsChanged(double counts);

	/// Monitors REIXSBeamline::bl()->valvesAndShutters()::beamOnChanged() to light up the "beam on" summary LED.
	void onBeamOnChanged(bool isOn);
};

#endif // REIXSSIDEBAR_H
