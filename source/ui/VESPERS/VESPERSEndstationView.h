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


#ifndef VESPERSENDSTATIONVIEW_H
#define VESPERSENDSTATIONVIEW_H

#include <QtGui/QWidget>

#include "beamline/VESPERS/VESPERSEndstation.h"
#include "ui/VESPERS/VESPERSMotorView.h"

#include "AMQEvents.h"

#include <QLineEdit>
#include <QStringList>
#include <QSlider>
#include <QToolButton>
#include <QMap>
#include <QComboBox>
#include <QPushButton>
#include <QProcess>
#include <QCheckBox>

/// This class is used to configure the endstation control.  As the endstation expands, the things that will need to be modified will also change.  Therefore, this class should expand with any expansion of the endstation class.
/// This will configure the buttons, as other things are added to the endstation it will do more.
/*! Current things that are configured:
		- Microscope button.
		- Safe distance for the CCD.
		- Limits on the different configurations.
*/
class VESPERSEndstationLimitsView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor for the configuration.
 	virtual ~VESPERSEndstationLimitsView();
	VESPERSEndstationLimitsView(QWidget *parent = 0);

signals:
	/// The limits and other possible configurations have changed.
	void configurationChanged();

protected slots:
	/// Saves the configuration in all the new line edits to the configuration file.
		void saveFile();

protected:
	/// Loads the configuration file and puts the contents into the line edits.
		void loadFile();

	QLineEdit *ccdLowLimit_;
	QLineEdit *ccdHighLimit_;
	QLineEdit *ccdHomePosition_;

	QLineEdit *microscopeInPosition_;
	QLineEdit *microscopeOutPosition_;
	QLineEdit *microscopeInStatus_;
	QLineEdit *microscopeOutStatus_;

	QLineEdit *vortexLowLimit_;
	QLineEdit *vortexHighLimit_;
	QLineEdit *vortexHomePosition_;

	QLineEdit *vortex4LowLimit_;
	QLineEdit *vortex4HighLimit_;
	QLineEdit *vortex4HomePosition_;

	QLineEdit *ccdBufferSafePosition_;
	QCheckBox *usingBuffer_;
	QCheckBox *at90Deg_;
};

/// This class shows all the general endstation controls.
class VESPERSEndstationView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Takes an instance of VESPERSEndstation and builds a view around it.
	VESPERSEndstationView(VESPERSEndstation *endstation, QWidget *parent = 0);
	/// Destructor.
	virtual ~VESPERSEndstationView();

	/// Returns whether or not the view is showing the normal sample stage motor or the y motor.  True is the normal stage and false is the y motor.
	bool usingNormalMotor() const { return usingNormal_; }

public slots:

protected slots:
	/// Handles setting the microscope slider without creating a signal/slot loop.
	void setMicroscopeLight(int val) { micLight_->blockSignals(true); micLight_->setValue(val); micLight_->blockSignals(false); }
	/// Used when the light bulb button is toggled.
	void lightBulbToggled(bool pressed);
	/// Used to initialize the laser power button.
	void laserPowerUpdate();

	// Slots handling the button clicks that will change the control window.
	/// Sets the window control with the appropriate settings.
	void setWindow(AMControl *control);

	/// Handles the CCD button being clicked.
	void ccdClicked()
	{
		if (!endstation_->microscopeInSafePosition()){

			AMControl *control = endstation_->control("Microscope motor");
			QMessageBox::warning(this, tr("Move Error"), tr("The microscope is in an unsafe position.  You must move the microscope to its %1 position (%2 %3) before you can move the microscope.").arg(endstation_->microscopeNames().second).arg(endstation_->getLimits(control).maximum()).arg(control->units()));
			return;
		}

		endstation_->setCurrent("CCD motor");
	}
	/// Handles the Microscope being clicked.
	void microscopeClicked()
	{
		if (!endstation_->ccdInSafePosition()){

			AMControl *control = endstation_->control("CCD motor");

			if (endstation_->heliumBufferAttached())
				QMessageBox::warning(this, tr("Move Error"), tr("The CCD is in an unsafe position.  You must move the CCD to %1 %2 before you can move the microscope.").arg(endstation_->ccdSafePositionwHeliumBuffer()).arg(control->units()));

			else
				QMessageBox::warning(this, tr("Move Error"), tr("The CCD is in an unsafe position.  You must move the CCD to %1 %2 before you can move the microscope.").arg(endstation_->getLimits(control).maximum()).arg(control->units()));

			return;
		}

		endstation_->setCurrent("Microscope motor");
	}
	/// Handles the 1-el vortex being clicked.
	void singleElClicked() { endstation_->setCurrent("1-Element Vortex motor"); }
	/// Handles the 4-el vortex being clicked.
	void fourElClicked() { endstation_->setCurrent("4-Element Vortex motor"); }

	// Slots handling the feedback updates from the PV.
	/// Handles the CCD distance update.
	void ccdUpdate(double val)
	{
		ccdButton_->setText(QString::number(val, 'f', 3) + " mm");

		if (!endstation_->ccdInSafePosition(val))
			microscopeButton_->setStyleSheet(" background-color: rgb(255,140,0); ");
		else
			microscopeButton_->setStyleSheet(this->styleSheet());
	}
	/// Handles the microscope distance update.
	void microscopeUpdate(double val)
	{
		microscopeButton_->setText(QString::number(val, 'f', 3) + " mm");

		if (!endstation_->microscopeInSafePosition(val))
			ccdButton_->setStyleSheet(" background-color: rgb(255,140,0); ");
		else
			ccdButton_->setStyleSheet(this->styleSheet());
	}
	/// Handles the 1-el vortex distance update.
	void singleElUpdate(double val) { singleElButton_->setText(QString::number(val, 'f', 3) + " mm"); }
	/// Handles the 4-el vortex distance update.
	void fourElUpdate(double val) { fourElButton_->setText(QString::number(val, 'f', 3) + " mm"); }
	/// Handles the laser position value changed signal.
	void onLaserDistanceChanged();

protected:
	// Microscope light setup.
	QSlider *micLight_;
	QToolButton *lightBulb_;

	// Bool holding whether or not the normal or y motor should be shown.
	bool usingNormal_;

	// Laser button.
	QToolButton *laserPowerButton_;

	// Config window.
	VESPERSEndstationLimitsView *config_;

	// Control Window.
	VESPERSMotorView *window_;

	// The buttons associated with each detector.
	QToolButton *ccdButton_;
	QToolButton *microscopeButton_;
	QToolButton *fourElButton_;
	QToolButton *singleElButton_;

	// The endstation model.
	VESPERSEndstation *endstation_;
};

#endif // VESPERSENDSTATIONVIEW_H
