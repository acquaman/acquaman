#ifndef SGMENERGYPOSITION_H
#define SGMENERGYPOSITION_H

#define SGMENERGY_MIN_VALUE 200
#define SGMENERGY_MAX_VALUE 3500

// Error validation messages
#define SGMENERGY_UNKNOWN_UNDULATOR_HARMONIC "Unknown undulator harmonic"
#define SGMENERGY_UNKNOWN_GRATING_TRANSLATION "Unknown grating translation"
#define SGMENERGY_INVALID_ENERGY_FOR_GRATING "Energy value is too small for current grating translation"


// Warning validation messages
#define SGMENERGY_UNDULATOR_TRACKING_OFF "Undulator tracking off: Optimal flux for energy may not be being achieved"
#define SGMENERGY_EXIT_SLIT_TRACKING_OFF "Exit slit position tracking off: Optimal flux for energy may not be being achieved"

#include <QObject>
#include <QStringList>
#include "SGMUndulatorSupport.h"
#include "SGMGratingSupport.h"
#include "SGMExitSlitSupport.h"
#include "util/AMValidator.h"

/*!
  * A class which represents a a state which the energy controls on the SGM beamline
  * can be in.
  * Note this class does not represent the actual energy controls, merely a possible
  * set of states.
  */
class SGMEnergyPosition : public QObject
{
	Q_OBJECT
public:

	/*!
	 * Creates an instance of an SGMEnergyPosition which is in the state represented
	 * by the provided information. The created info is assumed to be tracking the
	 * exit slit and undulator.
	 * \param gratingTranslation ~ The grating translation.
	 * \param gratingAngle ~ The encoder step value of the grating angle.
	 * \param undulatorHarmonic ~ The undulator harmonic.
	 * \param undulatorPosition ~ The undulator postion.
	 * \param undulatorOffset ~ An offset value used to detune the undulator.
	 * \param exitSlitPosition ~ The exit slit position.
	 * \param parent ~ The QObject parent.
	 */
	SGMEnergyPosition(SGMGratingSupport::GratingTranslation gratingTranslation,
	                  double gratingAngle,
	                  SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic,
	                  double undulatorPosition,
	                  double undulatorOffset,
	                  double exitSlitPosition,
	                  QObject* parent = 0);

	/*!
	 * Creates an instance of an SGMEnergyPosition whose componenets are tuned
	 * to provide the specified energy on the provided grating translation.
	 * \param requestedEnergy ~ The energy to tune the produced info for.
	 * \param gratingTranslation ~ The grating translation specified which is to
	 * be used to calculate the component positions for the given energy.
	 */
	SGMEnergyPosition(double requestedEnergy,
	                  SGMGratingSupport::GratingTranslation gratingTranslation,
	                  QObject* parent = 0);

	/*!
	 * Creates an instance of an SGMEnergyPosition whose components are tuned
	 * to provide the given energy, automatically determining the grating translation
	 * using the supplied optimization mode.
	 * \param requestedEnergy ~ The energy to tune the produced info for.
	 * \param gratingOptimizationMode ~ The means by which to determine the grating
	 * translation.
	 */
	SGMEnergyPosition(double requestedEnergy,
			  SGMGratingSupport::GratingTranslationOptimizationMode gratingOptimizationMode,
	                  QObject* parent = 0);


	/*!
	  * Creates a new SGMEnergyPosition which is a clone of the object is is
	  * called on, except that it represents a new item in the QObject sense.
	  * The created object has no QObject parent, and its resources are the
	  * responsibility of the caller.
	  */
	SGMEnergyPosition* clone() const;

	/*!
	  * Virtual destructor for an SGMMonochromatorInfo
	  */
	virtual ~SGMEnergyPosition() {}

	/*!
	  * The energy produced by the info's Grating translation and angle.
	  */
	double resultantEnergy() const;

	/*!
	  * The info's current grating translation selection
	  */
	SGMGratingSupport::GratingTranslation gratingTranslation() const;


	/*!
	 * The method to use to determine which grating translation to use to achieve
	 * an energy.
	 */
	SGMGratingSupport::GratingTranslationOptimizationMode gratingTranslationOptimizationMode() const;

	/*!
	  * The info's current grating angle.
	  */
	double gratingAngle() const;


	/*!
	 * Whether the undulator position is to track the energy to produce optimal
	 * flux. If undulator tracking is on the undulator will move as the energy is
	 * altered, if not it will remain at its current position
	 */
	bool isUndulatorTracking() const;

	/*!
	  * The info's current undulator harmonic.
	  */
	SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic() const;

	/*!
	 * Whether the undulator harmonic is automatically set each time an energy is
	 * requested.
	 */
	bool autoDetectUndulatorHarmonic() const;


	/*!
	  * The info's current undulator position.
	  */
	double undulatorPosition() const;

	/*!
	  * The info's current undulator offset.
	  */
	double undulatorOffset() const;

	/*!
	 * Whether the exit slit position is to track the energy to produce optimal
	 * flux. If exit slit tracking is on the exit will move as the energy is
	 * altered, if not it will remain at its current position
	 */
	bool isExitSlitPositionTracking() const;

	/*!
	  * The info's current exit slit position.
	  */
	double exitSlitPosition() const;

	/*!
	  * Convenience function for asking if the energy position currently has any
	  * errors. Equivalent to !errorValidator()->isValid()
	  */
	bool hasErrors() const;

	/*!
	  * Convenience function for asking if the energy position currently has any
	  * warnings. Equivalent to !warningValidator()->isValid()
	  */
	bool hasWarnings() const;

	/*!
	  * The error validator for the energy position.
	  */
	AMValidator* errorValidator() const;

	/*!
	  * The warning validator for the energy position.
	  */
	AMValidator* warningValidator() const;

signals:
	/// Signal indicating the grating translation has been altered.
	void gratingTranslationChanged(SGMGratingSupport::GratingTranslation gratingTranslation);

	/// Signal indicating tha the grating translation optimization mode has been altered.
	void gratingTranslationOptimizationModeChanged(SGMGratingSupport::GratingTranslationOptimizationMode gratingTranslationOptimizationMode);

	/// Signal indicating the grating angle has been altered.
	void gratingAngleChanged(double gratingAngle);

	/// Signal indicating the undulator tracking state has been altered.
	void undulatorTrackingChanged(bool isUndulatorTracking);

	/// Signal indicating the undulator harmonic has been altered.
	void undulatorHarmonicChanged(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic);

	/// Signal indicating the undulator position has been altered.
	void undulatorPositionChanged(double undulatorPosition);

	/// Signal indicating the undulator offset has been altered.
	void undulatorOffsetChanged(double undulatorOffset);

	/// Signal indicating the exit slit tracking state has been altered.
	void exitSlitTrackingChanged(bool isExitSlitPositionTracking);

	/// Signal indicating the exit slit position has been altered.
	void exitSlitPositionChanged(double exitSlitPosition);

	/*!
	  * Signal indicating that the energy produced by the component positions within
	  * the info have been altered.
	  */
	void energyChanged(double energy);

public slots:

	/*!
	  * Sets the info's grating translation selection.
	  */
	void setGratingTranslation(SGMGratingSupport::GratingTranslation gratingTranslation);

	/*!
	 * Sets the method used to determine which grating translation to use to achieve
	 * an energy.
	 * \param gratingTranslationOptimizationMode ~ The new grating translation optimization
	 * mode.
	 */

	void setGratingTranslationOptimizationMode(SGMGratingSupport::GratingTranslationOptimizationMode gratingTranslationOptimizationMode);
	/*!
	  * Sets the info's current grating angle.
	  */
	void setGratingAngle(double gratingAngle);

	/*!
	 * Sets whether the undulator position is to track the energy to produce
	 * optimal flux. If the undulator tracking is set from off to on, and the
	 * undulator position is not currently optimal for the energy, it will move
	 * to its optimal position.
	 * \param isTracking ~ Whether the undulator is to track energy.
	 */
	void setUndulatorTracking(bool isTracking);

	/*!
	  * Sets the info's current undulator harmonic.
	  */
	void setUndulatorHarmonic(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic);

	/*!
	 * Sets whether the undulator harmonic should be automatically set each time
	 * an energy is requested.
	 * \param autoDetect ~ Whether to automatically determine the undulator harmonic
	 * each time a new energy is requested.
	 */
	void setAutoDetectUndulatorHarmonic(bool autoDetect);

	/*!
	  * Sets the info's current undulator position.
	  */
	void setUndulatorPosition(double undulatorPosition);

	/*!
	  * Sets the info's current undulator offset.
	  */
	void setUndulatorOffset(double undulatorOffset);

	/*!
	 * Sets whether the exit slit position is to track the energy to produce
	 * optimal flux. If the exit tracking is set from off to on, and the
	 * exit slit position is not currently optimal for the energy, it will move
	 * to its optimal position.
	 * \param isTracking ~ Whether the exit slit is to track energy.
	 */
	void setExitSlitPositionTracking(bool isTracking);

	/*!
	  * Sets the info's current exit slit position.
	  */
	void setExitSlitPosition(double exitSlitPosition);

	/*!
	  * Requests the info set its component values to those which will produce the
	  * requested energy on the manually set grating translation selection.
	  * \param requestedEnergy ~ The energy (eVs) which the info is aiming for.
	  * \param gratingTranslation ~ The manually set grating translation to use
	  * to achieve the desired energy.
	  *
	  */
	void requestEnergy(double requestedEnergy, SGMGratingSupport::GratingTranslation gratingTranslation);

	/*!
	  * Requests the info set its comonent values to those which will produce the
	  * requested energy, automatically selecting a grating translation based on
	  * the provided optimization mode.
	  * \param requestedEnergy ~ The energy (eVs) which the info is aiming for.
	  * \param optimizationMode ~ Optional method to use to automatically select
	  * the grating translation. If none is selected ManualMode will be used.
	  */
	void requestEnergy(double requestedEnergy);
protected:

	/*!
	 * Optimizes the current undulator and exit slit position to produce optimal
	 * flux given the current energy produced by the grating.
	 */
	void optimizeForEnergy();

	/*!
	 * Helper method which checks the validity of the SGM Mono's state.
	 */
	void performValidation();

	SGMGratingSupport::GratingTranslation gratingTranslation_;
	SGMGratingSupport::GratingTranslationOptimizationMode gratingTranslationOptimizationMode_;
	double gratingAngle_;
	SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic_;
	bool autoDetectUndulatorHarmonic_;
	bool isUndulatorTracking_;
	double undulatorPosition_;
	double undulatorOffset_;
	bool isExitSlitPositionTracking_;
	double exitSlitPosition_;
	double requestedEnergy_;

	AMValidator* errorValidator_;
	AMValidator* warningValidator_;
};

#endif // SGMENERGYPOSITION_H
