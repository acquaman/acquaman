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

	/// The energy feedback PV ~ AM1611-4-I10:energy:eV:fbk
	AMControl* energyFeedback() const;

	/// The energy setpoint PV ~ AM1611-4-I10:energy:eV
	AMControl* energySetpoint() const;

	/// The energy stop PV ~ AM1611-4-I10:energy:stop
	AMControl* energyStop() const;

	/// The energy status PV ~ AM1611-4-I10:energy:status
	AMControl* energyStatus() const;

	/// The grating translation optimization mode feedback PV ~ AM1611-4-I10:energy:grating:translation:optimization:mode:fbk
	AMControl* gratingTranslationOptimizationModeFeedback() const;

	/// The grating translation optimization mode setpoint PV ~ AM1611-4-I10:energy:grating:translation:optimization:mode
	AMControl* gratingTranslationOptimizationModeSetpoint() const;

	/// The undulator offset PV ~ AM1611-4-I10:energy:undulator:offset:mm
	AMControl* undulatorOffset() const;

	/// The undulator harmonic feedback PV ~ AM1611-4-I10:energy:undulator:harmonic:fbk
	AMControl* undulatorHarmonicFeedback() const;

	/// The undulator harmonic setpoint PV ~ AM1611-4-I10:energy:undulator:harmonic
	AMControl* undulatorHarmonicSetpoint() const;

	/// The grating translation feedback PV ~ AM1611-4-I10:energy:grating:translation:feedback
	AMControl* gratingTranslationFeedback() const;

	/// The grating translation setpoint PV ~ AM1611-4-I10:energy:grating:translation
	AMControl* gratingTranslationSetpoint() const;

	/// The grating translation status PV ~ AM1611-4-I10:energy:grating:translation:status
	AMControl* gratingTranslationStatus() const;

	/// The grating translation stop PV ~ AM1611-4-I10:energy:grating:translation:stop
	AMControl* gratingTranslationStop() const;

	/// The undulator tracking PV ~ AM1611-4-I10:energy:undulator:tracking
	AMControl* undulatorTracking() const;

	/// The exit slit tracking PV ~ AM1611-4-I10:energy:exitslit:position:tracking
	AMControl* exitSlitTracking() const;
signals:
	/// Signal which indicates that the connected state of the contained controls
	/// has been altered.
	void connected(bool isConnected);
public slots:

protected:
	AMControlSet* controlSet_;
};

#endif // SGMNEWENERGYPVSET_H
