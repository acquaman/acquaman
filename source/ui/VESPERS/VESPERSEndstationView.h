#ifndef VESPERSENDSTATIONVIEW_H
#define VESPERSENDSTATIONVIEW_H

#include <QtGui/QWidget>

#include "beamline/VESPERS/VESPERSEndstation.h"
#include "ui/VESPERS/VESPERSMotorView.h"
#include "beamline/VESPERS/VESPERSBeamline.h"

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

private slots:
	/// Saves the configuration in all the new line edits to the configuration file.
	bool saveFile();

private:
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
	VESPERSEndstationView(QWidget *parent = 0);
	~VESPERSEndstationView();

protected:
	void closeEvent(QCloseEvent *e) { delete config_; e->accept(); }

private slots:
	/// Used to set the CCD Path when it changes from the program.
	void ccdPathEdited() { StringtoAMPV(ccdPath_, ccdPathEdit_->text()); }
	/// Used to set the CCD File name when it changes from the program.
	void ccdFileEdited() { StringtoAMPV(ccdFile_, ccdFileEdit_->text()); }
	/// Used to set the CCD Number when it changes from the program.
	void ccdNumberEdited() { ccdNumber_->setValue(ccdNumberEdit_->text()); }
	/// Used to set the updated path string to the line edit.
	void ccdPathUpdate(){ ccdPathEdit_->setText(AMPVtoString(ccdPath_)); }
	/// Used to set the updated name string to the line edit.
	void ccdFileUpdate() { ccdFileEdit_->setText(AMPVtoString(ccdFile_)); }
	/// Used to update the ccdNumber value.
	void ccdNumberUpdate(int newVal) { ccdNumberEdit_->setText(QString::number(newVal)); }
	/// Overloaded function.
	void ccdNumberUpdate() { ccdNumberEdit_->setText(QString::number(ccdNumber_->getInt())); }

	/// Used to initialize the Microscope Light slider.
	void micLightUpdate() { micLight_->setValue(micLightPV_->getInt()); }
	/// Used to disconnect the microscope light PV from the slider to prevent race conditions.
	void micLightSliderPressed() { disconnect(micLightPV_, SIGNAL(valueChanged()), this, SLOT(micLightUpdate())); }
	/// Used to reconnect the microscope light PV from the slider to get updates.
	void micLightSliderReleased() { connect(micLightPV_, SIGNAL(valueChanged()), this, SLOT(micLightUpdate())); }
	/// Used when the light bulb button is toggled.
	void lightBulbToggled(bool pressed);
	/// Used when the power for the laser is toggled.
	void laserPowerToggled(bool pressed);
	/// Used to initialize the laser power button.
	void laserPowerUpdate();

	// Slots handling the button clicks that will change the control window.
	/// Handles the CCD button being clicked.
	void ccdClicked()
	{
		if (!microscopeSafe_){
			QMessageBox::warning(this, tr("Move Error"), tr("The microscope is in an unsafe position.  You must move the microscope to its %1 position (%2 %3) before you can move the microscope.").arg(microscopeNames_.second).arg(softLimits_.value(microscopeControl_).second).arg(microscopeControl_->units()));
			return;
		}
		window_->setControl(ccdControl_, softLimits_.value(ccdControl_).first, softLimits_.value(ccdControl_).second);
	}
	/// Handles the Microscope being clicked.
	void microscopeClicked()
	{
		if (!ccdSafe_){
			QMessageBox::warning(this, tr("Move Error"), tr("The CCD is in an unsafe position.  You must move the CCD to %1 %2 before you can move the microscope.").arg(softLimits_.value(ccdControl_).second).arg(ccdControl_->units()));
			return;
		}
		window_->setControl(microscopeControl_, softLimits_.value(microscopeControl_).first, softLimits_.value(microscopeControl_).second, microscopeNames_.first, microscopeNames_.second);
	}
	/// Handles the 1-el vortex being clicked.
	void singleElClicked() { window_->setControl(singleElControl_, softLimits_.value(singleElControl_).first, softLimits_.value(singleElControl_).second); }
	/// Handles the 4-el vortex being clicked.
	void fourElClicked() { window_->setControl(fourElControl_, softLimits_.value(fourElControl_).first, softLimits_.value(fourElControl_).second); }
	/// Handles the focus being clicked.
	void focusClicked() { window_->setControl(focusControl_); }
	/// Handles updating the control window if the configuration changes.
	void updateControl();

	// Slots handling the feedback updates from the PV.
	/// Handles the CCD distance update.
	void ccdUpdate(double val)
	{
		ccdButton_->setText(QString::number(val, 'f', 3) + " " + ccdfbk_->units());

		if (endstation_->ccdInHomePosition(val)){

			microscopeButton_->setStyleSheet(" background-color: rgb(255,140,0); ");
			ccdSafe_ = false;
		}
		else {

			microscopeButton_->setStyleSheet(this->styleSheet());
			ccdSafe_ = true;
		}
	}
	/// Handles the microscope distance update.
	void microscopeUpdate(double val)
	{
		microscopeButton_->setText(QString::number(val, 'f', 3) + " " + microscopeControl_->units());

		if (!endstation_->microscopeInHomePosition(value)){

			ccdButton_->setStyleSheet(" background-color: rgb(255,140,0); ");
			microscopeSafe_ = false;
		}
		else {

			ccdButton_->setStyleSheet(this->styleSheet());
			microscopeSafe_ = true;
		}

	}
	/// Handles the 1-el vortex distance update.
	void singleElUpdate(double val) { singleElButton_->setText(QString::number(val, 'f', 3) + " " + singleElfbk_->units()); }
	/// Handles the 4-el vortex distance update.
	void fourElUpdate(double val) { fourElButton_->setText(QString::number(val, 'f', 3) + " " + fourElfbk_->units()); }
	/// Handles the focus distance update.
	void focusUpdate(double val) { focusButton_->setText(QString::number(val, 'f', 3) + " mm"); }
	/// Handles the connection of the filter set.
	void onFiltersConnected(bool isConnected);
	/// Sets the filter combo box based on original values at start up and if they are changed outside of the program.
	void onFiltersChanged();
	/// Starts up a detached process for the microscope screen.  Starts a detached process because the view for the microscope does not depend on the user interface to be active.
	void startMicroscope() { QProcess::startDetached("/home/vespers/bin/runCameraDisplay"); }
	/// Starts the IDA software.  This is temporary until the XAS software is replaced.
	void startXAS() { QProcess::startDetached("/home/vespers/bin/runIDA"); }

private:
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

	// Names of microscope positions.
	QPair<QString, QString> microscopeNames_;

	// Control Window.
	VESPERSMotorView *window_;

	// The buttons associated with each detector.
	QToolButton *ccdButton_;
	QToolButton *microscopeButton_;
	QToolButton *fourElButton_;
	QToolButton *singleElButton_;
	QToolButton *focusButton_;

	// Flags for message boxes.
	bool microscopeSafe_;
	bool ccdSafe_;

	// Filter combo box.
	QComboBox *filterComboBox_;

	// The endstation model.
	VESPERSEndstation *endstation_;
};

#endif // VESPERSENDSTATIONVIEW_H
