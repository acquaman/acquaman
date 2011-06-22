#ifndef VESPERSENDSTATIONVIEW_H
#define VESPERSENDSTATIONVIEW_H

#include <QtGui/QWidget>

#include "beamline/VESPERS/VESPERSEndstation.h"
#include "ui/VESPERS/VESPERSMotorView.h"

#include <QLineEdit>
#include <QStringList>
#include <QCloseEvent>
#include <QSlider>
#include <QToolButton>
#include <QMap>
#include <QComboBox>
#include <QPushButton>
#include <QProcess>

/// This class is used to configure the endstation control.  As the endstation expands, the things that will need to be modified will also change.  Therefore, this class should expand with any expansion of the endstation class.
/// This will configure the buttons, as other things are added to the endstation it will do more.
/*! Current things that are configured:
		- Microscope button.
		- Safe distance for the CCD.
		- Limits on the different configurations.
*/
class VESPERSEndstationConfigurationView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor for the configuration.
	VESPERSEndstationConfigurationView(QWidget *parent = 0);

signals:
	/// The limits and other possible configurations have changed.
	void configurationChanged();

protected slots:
	/// Saves the configuration in all the new line edits to the configuration file.
	bool saveFile();

protected:
	/// Loads the configuration file and puts the contents into the line edits.
	bool loadFile();

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
};

/// This class shows all the general endstation controls.
class VESPERSEndstationView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Takes the instance of VESPERSEndstation from the beamline and builds a view around it.  Sets up all the connections between the model.
	VESPERSEndstationView(QWidget *parent = 0);
	/// Destructor.
	~VESPERSEndstationView();

protected slots:
	/// Used to set the CCD Path when it changes from the program.
	void ccdPathEdited() { endstation_->setCCDPath(ccdPathEdit_->text()); }
	/// Used to set the CCD File name when it changes from the program.
	void ccdFileEdited() { endstation_->setCCDName(ccdFileEdit_->text()); }
	/// Used to set the CCD Number when it changes from the program.
	void ccdNumberEdited() { endstation_->setCCDNumber(ccdNumberEdit_->text().toInt()); }
	/// Used to update the ccdNumber value.
	void ccdNumberUpdate(int val) { ccdNumberEdit_->setText(QString::number(val)); }

	/// Handles setting the microscope slider without creating a signal/slot loop.
	void setMicroscopeLight(int val) { micLight_->blockSignals(true); micLight_->setValue(val); micLight_->blockSignals(false); }
	/// Used when the light bulb button is toggled.
	void lightBulbToggled(bool pressed);
	/// Used to initialize the laser power button.
	void laserPowerUpdate();

	// Slots handling the button clicks that will change the control window.
	/// Sets the window control with the appropriate settings.
	void setWindow(AMPVwStatusControl *control);

	/// Handles the CCD button being clicked.
	void ccdClicked()
	{
		if (!endstation_->ccdInHomePosition()){

			AMPVwStatusControl *control = endstation_->control("CCD");
			QMessageBox::warning(this, tr("Move Error"), tr("The microscope is in an unsafe position.  You must move the microscope to its %1 position (%2 %3) before you can move the microscope.").arg(endstation_->microscopeNames().second).arg(endstation_->getLimits(control).second).arg(control->units()));
			return;
		}

		endstation_->setCurrent("CCD");
	}
	/// Handles the Microscope being clicked.
	void microscopeClicked()
	{
		if (!endstation_->microscopeInHomePosition()){

			AMPVwStatusControl *control = endstation_->control("CCD");
			QMessageBox::warning(this, tr("Move Error"), tr("The CCD is in an unsafe position.  You must move the CCD to %1 %2 before you can move the microscope.").arg(endstation_->getLimits(control).second).arg(control->units()));
			return;
		}

		endstation_->setCurrent("Microscope");
	}
	/// Handles the 1-el vortex being clicked.
	void singleElClicked() { endstation_->setCurrent("1-Element Vortex"); }
	/// Handles the 4-el vortex being clicked.
	void fourElClicked() { endstation_->setCurrent("4-Element Vortex"); }
	/// Handles the focus being clicked.
	void focusClicked() { endstation_->setCurrent("Focus"); }

	// Slots handling the feedback updates from the PV.
	/// Handles the CCD distance update.
	void ccdUpdate(double val)
	{
		ccdButton_->setText(QString::number(val, 'f', 3) + " mm");

		if (!endstation_->ccdInHomePosition(val))
			microscopeButton_->setStyleSheet(" background-color: rgb(255,140,0); ");
		else
			microscopeButton_->setStyleSheet(this->styleSheet());
	}
	/// Handles the microscope distance update.
	void microscopeUpdate(double val)
	{
		microscopeButton_->setText(QString::number(val, 'f', 3) + " mm");

		if (!endstation_->microscopeInHomePosition(val))
			ccdButton_->setStyleSheet(" background-color: rgb(255,140,0); ");
		else
			ccdButton_->setStyleSheet(this->styleSheet());
	}
	/// Handles the 1-el vortex distance update.
	void singleElUpdate(double val) { singleElButton_->setText(QString::number(val, 'f', 3) + " mm"); }
	/// Handles the 4-el vortex distance update.
	void fourElUpdate(double val) { fourElButton_->setText(QString::number(val, 'f', 3) + " mm"); }
	/// Handles the focus distance update.
	void focusUpdate(double val) { focusButton_->setText(QString::number(val, 'f', 3) + " mm"); }

	/// Sets the filter combo box based on original values at start up and if they are changed outside of the program.
	void onFiltersChanged(int index) { filterComboBox_->blockSignals(true); filterComboBox_->setCurrentIndex(index); filterComboBox_->blockSignals(false); }
	/// Starts up a detached process for the microscope screen.  Starts a detached process because the view for the microscope does not depend on the user interface to be active.
	void startMicroscope() { QProcess::startDetached("/home/vespers/bin/runCameraDisplay"); }
	/// Starts the IDA software.  This is temporary until the XAS software is replaced.
	void startXAS() { QProcess::startDetached("/home/vespers/bin/runIDA"); }

protected:
	// CCD setup things.
	QLineEdit *ccdPathEdit_;
	QLineEdit *ccdFileEdit_;
	QLineEdit *ccdNumberEdit_;

	// Microscope light setup.
	QSlider *micLight_;
	QToolButton *lightBulb_;

	// Laser button.
	QToolButton *laserPowerButton_;

	// Config window.
	VESPERSEndstationConfigurationView *config_;

	// Control Window.
	VESPERSMotorView *window_;

	// The buttons associated with each detector.
	QToolButton *ccdButton_;
	QToolButton *microscopeButton_;
	QToolButton *fourElButton_;
	QToolButton *singleElButton_;
	QToolButton *focusButton_;

	// Filter combo box.
	QComboBox *filterComboBox_;

	// The endstation model.
	VESPERSEndstation *endstation_;
};

#endif // VESPERSENDSTATIONVIEW_H
