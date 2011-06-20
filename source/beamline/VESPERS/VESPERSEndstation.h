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

	/// Resets the pseudo-motor positions.
	void resetPseudoMotors() { resetPseudoMotors_->setValue(1); }
	/// Returns a bool on the state of the power of the microscope.  True for powered, false for un-powered.
	bool laserPowered() { return laserPower_->getInt() == 1 ? true : false; }
	/// Returns whether the microscope is in the home position.
	bool microscopeInHomePosition(double value) { return controlWithinTolerance(microscopeControl_, value, softLimits_.value(microscopeControl_).second); }
	/// Returns whether the CCD is in the home position.
	bool ccdInHomePosition(double value) { return controlWithinTolerance(ccdControl_, value, softLimits_.value(ccdControl_).second); }

signals:
	/// Notifier that the laser power changed.
	void laserPoweredChanged();
	/// Notifier that the filter index has changed.  This directly relates to the thickness of the filters in the same way as setFilterThickness (0 = 0um and 16 = 800 um).
	void filterThicknessChanged(int);

public slots:
	/// Toggles the current state of the laser power.
	void toggleLaserPower() { togglePV(laserPower_); }
	/// Loads the config file and then sets up the soft limits.
	bool loadConfiguration();
	/// Changes the filters placed in the beamline based on \code index.  The valid numbers are 0 - 16; 0 um to 800 um in 50 um intervals.
	void setFilterThickness(int index);

protected:
	/// Returns whether the \code control \code value is within tolerance of \code position.
	bool controlWithinTolerance(AMPVwStatusControl *control, double value, double position) { return fabs(value-position) < control->tolerance() ? true : false; }
	/// Helper function to properly toggle the filter PVs.  Takes an AMControl *, casts it to an AMPVControl * then toggles them.
	void togglePV(AMProcessVariable *pv) { pv->setValue(1); pv->setValue(0); }
	/// Converts the bizarre string output of the pv to a real QString.
	QString AMPVtoString(AMProcessVariable *pv);
	/// Converts the string to the array of integers it needs to be.
	void StringtoAMPV(AMProcessVariable *pv, QString toConvert);

	// Control map to soft limits.
	QMap<AMPVwStatusControl *, QPair<double, double> > softLimits_;

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
	AMProcessVariable *laserPower_;

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
	QMap<QString, AMProcessVariable *> filterMap_;
};

#endif // VESPERSENDSTATION_H
