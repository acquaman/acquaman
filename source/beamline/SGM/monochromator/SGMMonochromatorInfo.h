#ifndef SGMMONOCHROMATORINFO_H
#define SGMMONOCHROMATORINFO_H

#define MONOCHROMATOR_UNKNOWN_MIRROR_STRIPE "Unknown mirror stripe"
#define MONOCHROMATOR_UNKNOWN_UNDULATOR_HARMONIC "Unknown undulator harmonic"
#define MONOCHROMATOR_UNKNOWN_GRATING_TRANSLATION "Unknown grating translation"

#include <QObject>
#include <QStringList>
#include "SGMUndulatorSupport.h"
#include "SGMGratingSupport.h"
#include "util/AMValidator.h"

/*!
  * A class which represents a state in which a monochromater on the SGM beamline
  * can be in.
  * Note this class does not represent the actual monochromator itself, merely a
  * possible mono state.
  */
class SGMMonochromatorInfo : public QObject
{
    Q_OBJECT
public:

	/*!
	  * Enumerates the different means of automatically selecting a Grating Translation.
	  */
	enum GratingTranslationOptimizationMode {

		OptimizeFlux,				// Select grating which produces maximum flux for a given energy.
		OptimizeResolution,			// Select highest grating which will produce reasonable flux at a given energy.
		OptimizeMinimalMovement		// Stick with the current grating.
	};

	/*!
	  * Creates a new instance of an SGMMonochromatorInfo, which is in an invalid
	  * state.
	  */
	explicit SGMMonochromatorInfo(QObject *parent = 0);

	/*!
	  * Virtual destructor for an SGMMonochromatorInfo
	  */
	virtual ~SGMMonochromatorInfo() {}

	/*!
	  * Whether the info has errors with its current settings.
	  */
	bool hasErrors() const;

	/*!
	  * Whether the info has warnings with its current settings.
	  */
	bool hasWarnings() const;

	/*!
	  * The error messages related to the valid state of the info. If the info is
	  * valid the empty list will be returned.
	  */
	QStringList errorMessages() const;

	/*!
	  * The warning messages related to the valid state of the info.
	  */
	QStringList warningMessages() const;

	/*!
	  * The energy produced by the info's Grating translation and angle.
	  */
	double resultantEnergy() const;

	/*!
	  * The info's current grating translation selection
	  */
	SGMGratingSupport::GratingTranslation gratingTranslation() const;

	/*!
	  * Sets the info's grating translation selection.
	  */
	void setGratingTranslation(SGMGratingSupport::GratingTranslation gratingTranslation);

	/*!
	  * The info's current grating angle.
	  */
	double gratingAngle() const;

	/*!
	  * Sets the info's current grating angle.
	  */
	void setGratingAngle(double gratingAngle);

	/*!
	  * The info's current undulator harmonic.
	  */
	SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic() const;

	/*!
	  * Sets the info's current undulator harmonic.
	  */
	void setUndulatorHarmonic(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic);

	/*!
	  * The info's current undulator position.
	  */
	double undulatorPosition() const;

	/*!
	  * Sets the info's current undulator position.
	  */
	void setUndulatorPosition(double undulatorPosition);

	/*!
	  * The info's current undulator offset.
	  */
	double undulatorOffset() const;

	/*!
	  * Sets the info's current undulator offset.
	  */
	void setUndulatorOffset(double undulatorOffset);

	/*!
	  * The info's current exit slit position.
	  */
	double exitSlitPosition() const;

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
	  * the grating translation. If none is selected OptimizeMinimalMovement will
	  * be used.
	  */
	void requestEnergy(double requestedEnergy, GratingTranslationOptimizationMode optimizationMode = OptimizeMinimalMovement);

	static void testValue(const QString &name, double requiredEnergy, SGMGratingSupport::GratingTranslation gratingTranslationSelection, double expectedAngleEncoderTarget, double expectedExitSlitPosition, double expectedUndulatorPosition, double expectedUndulatorStepValue);

	static void testValues();

signals:
	/// Signal indicating the grating translation has been altered.
	void gratingTranslationChanged(SGMGratingSupport::GratingTranslation gratingTranslation);

	/// Signal indicating the grating angle has been altered.
	void gratingAngleChanged(double gratingAngle);

	/// Signal indicating the undulator harmonic has been altered.
	void undulatorHarmonicChanged(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic);

	/// Signal indicating the undulator position has been altered.
	void undulatorPositionChanged(double undulatorPosition);

	/// Signal indicating the undulator offset has been altered.
	void undulatorOffsetChanged(double undulatorOffset);

	/// Signal indicating the exit slit position has been altered.
	void exitSlitPositionChanged(double exitSlitPosition);

	/*!
	  * Signal indicating that the energy produced by the component positions within
	  * the info have been altered.
	  */
	void energyChanged(double energy);

	/*!
	  * Signal indicating that the valid state of the info has altered.
	  */
	void errorStateChanged(bool errorState);

	/*!
	  * Signal indicating that the number of errors related to the info has altered.
	  */
	void errorCountChanged();

	/*!
	  * Signal indicating that the warning state of the info has altered.
	  */
	void warningStateChanged(bool warningState);

	/*!
	  * Signal indicating that the number of warnings related to the info has
	  * altered.
	  */
	void warningCountChanged();
public slots:


protected:
	/*!
	  * Helper method which returns the best fit grating translation for a given
	  * energy and optimization mode.
	  * \param requestedEnergy ~ The energy to optimize the grating translation for.
	  * \param optimizationMode ~ The optimization criteria for selecting a grating
	  * translation.
	  */
	SGMGratingSupport::GratingTranslation optimizedGrating(double requestedEnergy,
														   GratingTranslationOptimizationMode optimizationMode);

	/*!
	  * Helper method used to determine the energy produced by a given grating
	  * translation and angle encoder target.
	  * \param gratingTranslationSelection ~ The grating translation to use in
	  * calculating the produced energy.
	  * \param gratingAngleEncoderTarget ~ The encoder target of the grating angle
	  * to use in calculating the energy produced.
	  */
	double energyFromGrating(SGMGratingSupport::GratingTranslation gratingTranslationSelection,
										double gratingAngleEncoderTarget) const;

	/*!
	  * Helper method which calculates the grating angle encoder target required
	  * to produce the provided energy when using the provided grating translation.
	  * \param gratingTranslationSelection ~ The grating translation under which
	  * the energy is required.
	  * \param energy ~ The energy for which the grating angle encoder target is
	  * required.
	  */
	double gratingAngleFromEnergy(SGMGratingSupport::GratingTranslation gratingTranslationSelection,
											double energy) const;

	/*!
	  * Helper method used to calculate the undulator (step) postition required
	  * to optimize flux for a given energy and harmonic taking into account the
	  * provided undulator detune offset.
	  * \param energy ~ The energy which the calculated undulator step position
	  * is to be optimized for.
	  * \param undulatorHarmonic ~ The undulator harmonic to use in calculating
	  * the optimized undulator position.
	  * \param undulatorOffset ~ A detune value offset for the produced undulator
	  * position.
	  */
	double optimizedUndulatorPosition(double energy,
									  SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic,
									  double undulatorOffset) const;

	/*!
	  * Helper method used to calculate the exit slit position required to optimize
	  * flux for a given energy and grating translation.
	  * \param gratingTranslationSelection ~ The grating translation used in
	  * calculating the optimized exit slit position.
	  * \param energy ~ The energy to optimize the exit slit for.
	  */
	double optimizedExitSlitPosition(SGMGratingSupport::GratingTranslation gratingTranslationSelection,
									 double energy) const;

	SGMGratingSupport::GratingTranslation gratingTranslation_;
	double gratingAngle_;
	SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic_;
	double undulatorPosition_;
	double undulatorOffset_;
	double exitSlitPosition_;

	AMValidator errorValidator_;
	AMValidator warningValidator_;
};

#endif // SGMMONOCHROMATORINFO_H
