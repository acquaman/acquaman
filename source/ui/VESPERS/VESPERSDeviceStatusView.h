/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef VESPERSDEVICESTATUSVIEW_H
#define VESPERSDEVICESTATUSVIEW_H

#include <QWidget>
#include <QButtonGroup>
#include <QToolButton>

class VESPERSDeviceStatusView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.  Builds the device status view.
	explicit VESPERSDeviceStatusView(QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Handles the pressure status signal from the VESPERSBeamline.
	void onPressureStatusChanged(bool status) { device_->button(1)->setPalette(QPalette(status == true ? palette() : QPalette(Qt::red))); }
	/// Handles the valve status signal from the VESPERSBeamline.
	void onValveStatusChanged(bool status) { device_->button(2)->setPalette(QPalette(status == true ? palette() : QPalette(Qt::red))); }
	/// Handles the ion pump status signal from the VESPERSBeamline.
	void onIonPumpStatusChanged(bool status) { device_->button(3)->setPalette(QPalette(status == true ? palette() : QPalette(Qt::red))); }
	/// Handles the temperature status signal from the VESPERSBeamline.
	void onTemperatureStatusChanged(bool status) { device_->button(0)->setPalette(QPalette(status == true ? palette() : QPalette(Qt::red))); }
	/// Handles the flow switch status signal from the VESPERSBeamline.
	void onFlowSwitchStatusChanged(bool status) { device_->button(4)->setPalette(QPalette(status == true ? palette() : QPalette(Qt::red))); }
	/// Handles the flow transducer status signal from the VESPERSBeamline.
	void onFlowTransducerStatusChanged(bool status) { device_->button(5)->setPalette(QPalette(status == true ? palette() : QPalette(Qt::red))); }

protected:
	/// The button group that holds the buttons in the homemade tool bar.
	QButtonGroup *device_;
};

#endif // VESPERSDEVICESTATUSVIEW_H
