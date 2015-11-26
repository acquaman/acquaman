#ifndef SGMENERGYCONTROLSET_H
#define SGMENERGYCONTROLSET_H

#include "beamline/AMControlSet.h"

class SGMEnergyPosition;
class AMValidator;
/*!
  * A class which contains PV controls for the energy components of the SGM Beamline.
  * Also contains a validation mechanism which monitors the state of the energy
  * components for errors and warnings.
  */
class SGMEnergyControlSet : public AMControlSet
{
	Q_OBJECT
public:
	/*!
	  * Creates a new instance of an SGMEnergyControlSet.
	  */
	explicit SGMEnergyControlSet(QObject *parent = 0);

	/*!
	  * The Energy master control for the beamline.
	  */
	AMControl* energy() const;

	/*!
	  * The control for the target energy of a continuous energy motion.
	  */
	AMControl* energyTrajectoryTarget() const;

	/*!
	  * The control for the time of a continuous motion should take.
	  */
	AMControl* energyTrajectoryTime() const;

	/*!
	  * The control for commencing a continuous motion.
	  */
	AMControl* energyTrajectoryStart() const;

	/*!
	  * The status control of the energy.
	  */
	AMControl* energyStatus() const;

	/*!
	  * The grating angle control. The grating angle determines (along with the
	  * grating translation) the energy the beamline produces. This control can
	  * be moved to manually produce different energies, without the coordinated
	  * movement of the master energy PV.
	  */
	AMControl* gratingAngle() const;

	/*!
	  * The grating translation control. The grating angle determines (along with
	  * the grating angle) the energy the beamline produces.
	  * NOTE: Changing the grating translation is only possible when the optimization
	  * mode is set to manual.
	  */
	AMControl* gratingTranslation() const;

	/*!
	  * The grating translation optimization mode. When this is set to a setting
	  * other than manual, the optimal grating translation to use on a given energy
	  * is selected prior to any energy move.
	  */
	AMControl* gratingTranslationOptimization() const;

	/*!
	  * The undulator position control.
	  */
	AMControl* undulatorPosition() const;

	/*!
	  * A detune offset value which is applied to the undulator position targeted
	  * when the energy is moved.
	  */
	AMControl* undulatorDetuneOffset() const;

	/*!
	  * The control for the harmonic used by the undulator.
	  */
	AMControl* undulatorHarmonic() const;

	/*!
	  * The control which determines whether the undulator position  is to be moved
	  * to a position which will optimize flux each time a new energy is targeted.
	  */
	AMControl* undulatorTracking() const;

	/*!
	  * The exit slit position control.
	  */
	AMControl* exitSlitPosition() const;

	/*!
	  * The control which determines whether the exit slit position  is to be moved
	  * to a position which will optimize flux each time a new energy is targeted.
	  */
	AMControl* exitSlitPositionTracking() const;

	/*!
	  * The validator for the error state of the energy controls.
	  */
	AMValidator* errorValidator() const;

	/*!
	  * The validator for the warning state of the energy controls.
	  */
	AMValidator* warningValidator() const;
signals:
	/// Signal indicating that the control set is fully initialized.
	void initialized();
public slots:
protected slots:
	void onAllConnected(bool);

	void onEnergyPVValueChanged(double);

	void onGratingAnglePVValueChanged(double);

	void onGratingTranslationPVValueChanged(double);

	void onGratingtranslationOptimizationPVValueChanged(double);

	void onUndulatorPositionPVValueChanged(double);

	void onUndulatorDetuneOffsetPVValueChanged(double);

	void onUndulatorHarmonicPVValueChanged(double);

	void onUndulatorTrackingPVValueChanged(double);

	void onExitSlitPositionPVValueChanged(double);

	void onExitSlitTrackingPVValueChanged(double);
protected:
	SGMEnergyPosition* energyPositionValidator_;
};

#endif // SGMENERGYCONTROLSET_H
