#ifndef SGMNEWENERGYPVSET_H
#define SGMNEWENERGYPVSET_H

#include <QObject>
class AMControlSet;
class AMControl;
/*!
  * Convenience class which holds control interfaces for all the new PVs for the
  * energy coordinator.
  */
class SGMNewEnergyPVSet : public QObject
{
	Q_OBJECT
public:
	/*!
	  * Creates a new energy pv set which contains references to all the new energy
	  * PVs
	  */
	explicit SGMNewEnergyPVSet(QObject *parent = 0);

	/// Whether all the controls contained within the set are connected.
	bool isConnected() const;

	// Energy
	/////////////////////////////////////////
	/// The energy feedback PV ~ AM1611-4-I10:energy:eV:fbk
	AMControl* energyFeedback() const;

	/// The energy setpoint PV ~ AM1611-4-I10:energy:eV
	AMControl* energySetpoint() const;

	/// The energy stop PV ~ AM1611-4-I10:energy:stop
	AMControl* energyStop() const;

	/// The energy status PV ~ AM1611-4-I10:energy:status
	AMControl* energyStatus() const;

	/// The energy trajectory target PV ~ AM1611-4-I10:energy:trajectory:target:eV
	AMControl* energyTrajectoryTarget() const;

	/// The energy trajectory time PV ~ AM1611-4-I10:energy:trajectory:time:s"
	AMControl* energyTrajectoryTime() const;

	/// The energy trajectory start PV ~ AM1611-4-I10:energy:trajectory:start
	AMControl* energyTrajectoryStart() const;

	// Grating - Angle
	/////////////////////////////////////////
	/// The grating angle feedback PV ~ AM1611-4-I10:energy:grating:angle:steps:fbk
	AMControl* gratingAngleFeedback() const;

	/// The grating angle setpoint PV ~ AM1611-4-I10:energy:grating:angle:steps
	AMControl* gratingAngleSetpoint() const;

	/// The grating angle status PV ~ AM1611-4-I10:energy:grating:angle:status
	AMControl* gratingAngleStatus() const;

	/// The grating angle stop PV ~ AM1611-4-I10:energy:grating:angle:stop
	AMControl* gratingAngleStop() const;

	// Grating - Translation
	/////////////////////////////////////////
	/// The grating translation feedback PV ~ AM1611-4-I10:energy:grating:translation:feedback
	AMControl* gratingTranslationFeedback() const;

	/// The grating translation setpoint PV ~ AM1611-4-I10:energy:grating:translation
	AMControl* gratingTranslationSetpoint() const;

	/// The grating translation status PV ~ AM1611-4-I10:energy:grating:translation:status
	AMControl* gratingTranslationStatus() const;

	/// The grating translation stop PV ~ AM1611-4-I10:energy:grating:translation:stop
	AMControl* gratingTranslationStop() const;

	/// The grating translation optimization mode feedback PV ~ AM1611-4-I10:energy:grating:translation:optimization:fbk
	AMControl* gratingTranslationOptimizationModeFeedback() const;

	/// The grating translation optimization mode setpoint PV ~ AM1611-4-I10:energy:grating:translation:optimization
	AMControl* gratingTranslationOptimizationModeSetpoint() const;

	// Undulator - Position
	/////////////////////////////////////////
	/// The undulator position feedback PV ~ AM1611-4-I10:energy:undulator:mm:fbk
	AMControl* undulatorPositionFeedback() const;

	/// The undulator position setpoint PV ~ AM1611-4-I0:energy:undulator:mm
	AMControl* undulatorPositionSetpoint() const;

	/// The undulator position status PV ~ AM1611-4-I10:energy:undulator:status
	AMControl* undulatorPositionStatus() const;

	/// The undulator position stop PV ~ AM1611-4-I10:energy:undulator:stop
	AMControl* undulatorPositionStop() const;

	// Undulator - Offset
	/////////////////////////////////////////
	/// The undulator offset PV ~ AM1611-4-I10:energy:undulator:offset:mm
	AMControl* undulatorOffset() const;

	// Undulator - Harmonic
	/////////////////////////////////////////
	/// The undulator harmonic feedback PV ~ AM1611-4-I10:energy:undulator:harmonic:fbk
	AMControl* undulatorHarmonicFeedback() const;

	/// The undulator harmonic setpoint PV ~ AM1611-4-I10:energy:undulator:harmonic
	AMControl* undulatorHarmonicSetpoint() const;

	// Undulator - Tracking
	/////////////////////////////////////////
	/// The undulator tracking PV ~ AM1611-4-I10:energy:undulator:tracking
	AMControl* undulatorTracking() const;

	// Exit Slit - Position
	/////////////////////////////////////////
	/// The exit slit position feedback PV ~ AM1611-4-I10:energy:exitslit:position:mm:fbk
	AMControl* exitSlitPositionFeedback() const;

	/// The exit slit position setpoint PV ~ AM1611-4-I0:energy:exitslit:position:mm
	AMControl* exitSlitPositionSetpoint() const;

	/// The exit slit position status PV ~ AM1611-4-I10:energy:exitslit:position:status
	AMControl* exitSlitPositionStatus() const;

	/// The exit slit position stop PV ~ AM1611-4-I10:energy:exitslit:position:stop
	AMControl* exitSlitPositionStop() const;

	// Exit Slit - Position Tracking
	/////////////////////////////////////////
	/// The exit slit tracking PV ~ AM1611-4-I10:energy:exitslit:position:tracking
	AMControl* exitSlitPositionTracking() const;
signals:
	/// Signal which indicates that the connected state of the contained controls
	/// has been altered.
	void connected(bool isConnected);
public slots:

protected:
	AMControlSet* controlSet_;
};

#endif // SGMNEWENERGYPVSET_H
