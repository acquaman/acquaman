#ifndef VESPERSENDSTATION_H
#define VESPERSENDSTATION_H

#include <QObject>
#include <QMap>

#include <beamline/AMControl.h>

class VESPERSEndstation : public QObject
{
	Q_OBJECT
public:
	/// The constructor.  Builds and encapsulates many of the controls used for the endstation.
	explicit VESPERSEndstation(QObject *parent = 0);

	/// Returns a bool on the state of the power of the microscope.  True for powered, false for un-powered.
	bool laserPowered() { return (int)laserPower_->value() == 1 ? true : false; }
	/// Returns whether the microscope is in the home position.
	bool microscopeInHomePosition(double value) { return controlWithinTolerance(microscopeControl_, value, softLimits_.value(microscopeControl_).second); }
	/// Overloaded function.  Uses current value.
	bool microscopeInHomePosition() { return controlWithinTolerance(microscopeControl_, microscopeControl_->value(), softLimits_.value(microscopeControl_).second); }
	/// Returns whether the CCD is in the home position.
	bool ccdInHomePosition(double value) { return controlWithinTolerance(ccdControl_, value, softLimits_.value(ccdControl_).second); }
	/// Overloaded function.  Uses current value.
	bool ccdInHomePosition() { return controlWithinTolerance(ccdControl_, ccdControl_->value(), softLimits_.value(ccdControl_).second); }
	/// Enables/Disables the microscope PV by blocking signals.
	void lightEnabled(bool on) { micLightPV_->blockSignals(!on); }
	/// Returns the current control being focused on.
	AMPVwStatusControl *current() const { return current_; }
	/// Returns a control based on the name.  Returns 0 if invalid.
	AMPVwStatusControl *control(QString name) const;
	/// Returns the soft limits for the given control (if any) as a QPair<double, double>.
	QPair<double, double> getLimits(AMPVwStatusControl *control) { return qMakePair(softLimits_.value(control).first, softLimits_.value(control).second); }
	/// Returns the microscope setpoint names as a QPair<QString, QString>.
	QPair<QString, QString> microscopeNames() const { return microscopeNames_; }

signals:
	/// Notifier that the laser power changed.
	void laserPoweredChanged();
	/// Notifier that the filter index has changed.  This directly relates to the thickness of the filters in the same way as setFilterThickness (0 = 0um and 16 = 800 um).
	void filterThicknessChanged(int);
	/// Notifier that the light intensity has changed.
	void lightIntensityChanged(int);
	/// The current control has either been updated or changed.  Passes the pointer to the control.
	void currentControlChanged(AMPVwStatusControl *);
	/// The CCD feedback motor position changed notification.
	void ccdFbkChanged(double);
	/// The focus feeback motor position changed notification.
	void focusFbkChanged(double);
	/// The microscope feedback motor position changed notification.
	void microscopeFbkChanged(double);
	/// The single element motor position changed notification.
	void singleElFbkChanged(double);
	/// The four element motor position changed notification.
	void fourElFbkChanged(double);
	/// Notifier that the CCD path has changed.
	void ccdPathChanged(QString);
	/// Notifier that the CCD file name has changed.
	void ccdNameChanged(QString);
	/// Notifier that the CCD number has changed.
	void ccdNumberChanged(int);

public slots:
	/// Sets the current control that should be focused on.  It will find the correct control based on the name.  Control is set to 0 if invalid name is given.
	void setCurrent(QString name);
	/// Resets the pseudo-motor positions.
	void resetPseudoMotors() { resetPseudoMotors_->setValue(1); }
	/// Toggles the current state of the laser power.
	void toggleLaserPower() { toggleControl(laserPower_); emit laserPoweredChanged(); }
	/// Loads the config file and then sets up the soft limits.
	bool loadConfiguration();
	/// Changes the filters placed in the beamline based on \code index.  The valid numbers are 0 - 16; 0 um to 800 um in 50 um intervals.
	void setFilterThickness(int index);
	/// Set the value for the microscope.  Must be between 0 and 100.
	void setLightIntensity(int intensity) { if (micLightPV_->getInt() != intensity) micLightPV_->setValue(intensity); }
	/// Sets the CCD file path.
	void setCCDPath(QString path) { StringtoAMPV(ccdPath_, path); }
	/// Sets the CCD file name.
	void setCCDName(QString name) { StringtoAMPV(ccdFile_, name); }
	/// Sets the CCD file number.
	void setCCDNumber(int number) { ccdNumber_->setValue(number); }

protected slots:
	/// Determines if the filters are all connected.  They shouldn't be accessed until they are all connected.
	void onFiltersConnected();
	/// Helper slot that emits a signal with the current index related to the thickness of the filters.
	void onFiltersChanged();
	/// Handles updating the control window if the configuration changes.
	void updateControl(AMPVwStatusControl *control);
	/// Handles the CCD path update.
	void onCCDPathChanged() { emit ccdPathChanged(AMPVtoString(ccdPath_)); }
	/// Handles the CCD name update.
	void onCCDNameChanged() { emit ccdNameChanged(AMPVtoString(ccdFile_)); }

protected:
	/// Returns whether the \code control \code value is within tolerance of \code position.
	bool controlWithinTolerance(AMPVwStatusControl *control, double value, double position) { return fabs(value-position) < control->tolerance() ? true : false; }
	/// Helper function to properly toggle the filter PVs.  Takes an AMControl *, casts it to an AMPVControl * then toggles them.
	void toggleControl(AMPVControl *control) { control->move(1); control->move(0); }
	/// Converts the bizarre string output of the pv to a real QString.
	QString AMPVtoString(AMProcessVariable *pv);
	/// Converts the string to the array of integers it needs to be.
	void StringtoAMPV(AMProcessVariable *pv, QString toConvert);

	// Holds the previous state of the filter connectivity.
	bool wasConnected_;

	// Control map to soft limits.
	QMap<AMPVwStatusControl *, QPair<double, double> > softLimits_;
	// Names of microscope positions.
	QPair<QString, QString> microscopeNames_;

	// Control pointers.
	// The current control being pointed to.
	AMPVwStatusControl *current_;

	// The controls used for the control window.
	AMPVwStatusControl *ccdControl_;
	AMPVwStatusControl *microscopeControl_;
	AMPVwStatusControl *fourElControl_;
	AMPVwStatusControl *singleElControl_;
	AMPVwStatusControl *focusControl_;

	// The controls that have the feedback value used for the button.  The microscope doesn't need one because it's encoder doesn't work.
	AMReadOnlyPVControl *ccdfbk_;
	AMReadOnlyPVControl *fourElfbk_;
	AMReadOnlyPVControl *singleElfbk_;
	AMReadOnlyPVControl *focusfbk_;

	// Microscope light PV.
	AMProcessVariable *micLightPV_;

	// Laser power control.
	AMPVControl *laserPower_;

	// Various CCD file path PVs.
	AMProcessVariable *ccdPath_;
	AMProcessVariable *ccdFile_;
	AMProcessVariable *ccdNumber_;

	// The pseudo-motor reset PV.
	AMProcessVariable *resetPseudoMotors_;

	// Filter process variables.
	AMProcessVariable *filter250umA_;
	AMProcessVariable *filter250umB_;
	AMProcessVariable *filter100umA_;
	AMProcessVariable *filter100umB_;
	AMProcessVariable *filter50umA_;
	AMProcessVariable *filter50umB_;
	AMProcessVariable *filterShutterUpper_;
	AMProcessVariable *filterShutterLower_;

	// A list of all the filters, but not the upper or lower shutters.
	QMap<QString, AMPVControl *> filterMap_;
};

#endif // VESPERSENDSTATION_H
