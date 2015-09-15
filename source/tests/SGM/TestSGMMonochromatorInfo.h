#ifndef TESTSGMMONOCHROMATORINFO_H
#define TESTSGMMONOCHROMATORINFO_H

#include <QStringList>
#include "beamline/SGM/monochromator/SGMGratingSupport.h"

class TestSGMMonochromatorInfo
{
public:
    TestSGMMonochromatorInfo();

    QStringList testResults() const;

    void performTests();

protected:
    void testInitialization();

    void testIndividualRequestEnergy(const QString &name,
                                     double requiredEnergy,
                                     SGMGratingSupport::GratingTranslation gratingTranslationSelection,
                                     double expectedAngleEncoderTarget,
                                     double expectedExitSlitPosition,
                                     double expectedUndulatorPosition,
                                     double expectedUndulatorStepValue);

    void testRequestEnergy();

    void testResultantEnergy();

    QStringList testResults_;
};

#endif // TESTSGMMONOCHROMATORINFO_H
