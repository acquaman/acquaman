#ifndef SGMENERGYPOSITIONTEST_H
#define SGMENERGYPOSITIONTEST_H

#include <QStringList>
#include "beamline/SGM/energy/SGMGratingSupport.h"

/*!
 * A class which performs basic tests on the SGNEnergyPosition class
 */
class SGMEnergyPositionTest
{
public:
	SGMEnergyPositionTest();

	/*!
	 * A list of the test results
	 */
	QStringList testResults() const;

	/*!
	 * Runs all the test functions
	 */
	void performTests();

protected:

	/*!
	 * Tests an individual requestEnergy() function call, ensuring the results
	 * meet those of the passed expected values
	 */
	void testIndividualRequestEnergy(const QString &name,
	                                 double requiredEnergy,
	                                 SGMGratingSupport::GratingTranslation gratingTranslationSelection,
	                                 double expectedAngleEncoderTarget,
	                                 double expectedExitSlitPosition,
	                                 double expectedUndulatorPosition,
	                                 double expectedUndulatorStepValue);

	/*!
	 * Tests the requestEnergy() function by making a series of calls to
	 * testIndividualRequestEnergy() with various known values.
	 */
	void testRequestEnergy();

	/*!
	 * Tests the resultant energy function. Ensures that the reverse calculation
	 * of grating to energy is functioning.
	 */
	void testResultantEnergy();

	QStringList testResults_;
};

#endif // SGMENERGYPOSITIONTEST_H
