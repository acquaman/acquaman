#include "SGMMonochromatorInfoTest.h"
#include "beamline/SGM/monochromator/SGMMonochromatorInfo.h"
SGMMonochromatorInfoTest::SGMMonochromatorInfoTest()
{

}

QStringList SGMMonochromatorInfoTest::testResults() const
{
    return testResults_;
}

void SGMMonochromatorInfoTest::performTests()
{
    testResults_.clear();
    testRequestEnergy();
    testResultantEnergy();
}

void SGMMonochromatorInfoTest::testIndividualRequestEnergy(const QString &name,
                                                           double requiredEnergy,
                                                           SGMGratingSupport::GratingTranslation gratingTranslationSelection,
                                                           double expectedAngleEncoderTarget,
                                                           double expectedExitSlitPosition,
                                                           double expectedUndulatorPosition,
                                                           double expectedUndulatorStepValue)
{
    SGMMonochromatorInfo testMonoInfo(requiredEnergy, gratingTranslationSelection);

    // Compute all the required values
    double angleEncoderTarget = testMonoInfo.gratingAngle();
    double requestedUndulatorPosition = testMonoInfo.undulatorPosition();
    double requestedUndulatorStepValue = SGMUndulatorSupport::undulatorStepFromPosition(requestedUndulatorPosition, 12.7806, -153573);
    double requestedExitSlitPosition = testMonoInfo.exitSlitPosition();
    double absDifference;
    double average;
    double percentageDifference;
    QString gratingString;
    switch(gratingTranslationSelection) {
    case SGMGratingSupport::LowGrating:
        gratingString = "Low Grating";
        break;
    case SGMGratingSupport::MediumGrating:
        gratingString = "Medium Grating";
        break;
    case SGMGratingSupport::HighGrating:
        gratingString = "High Grating";
        break;
    case SGMGratingSupport::UnknownGrating:
        gratingString = "Unknown Grating";
    }

    if(testMonoInfo.hasErrors()) {
        testResults_ << QString("%1 at %2ev has errors - Can't test").arg(name).arg(requiredEnergy);
        testResults_.append(testMonoInfo.errorMessage());
        return;
    }

    if(testMonoInfo.hasWarnings()) {
        testResults_ << QString("%1 at %2eV has warnings - Tests continue").arg(name).arg(requiredEnergy);
        testResults_ << testMonoInfo.warningMessage();
    }

    // Do comparisons, outputting if the percentage difference from expected is > 1%
    absDifference = qAbs(expectedAngleEncoderTarget - angleEncoderTarget);
    average = (expectedAngleEncoderTarget + angleEncoderTarget) / 2;
    percentageDifference = (absDifference / average) * 100;
    if(percentageDifference >= 1) {
        testResults_ << QString("%1 at %2eV (%3) Grating test failed - Expected:%4 Actual:%5").arg(name).arg(requiredEnergy).arg(gratingString).arg(expectedAngleEncoderTarget).arg(angleEncoderTarget);
    }

    absDifference = qAbs(expectedUndulatorPosition - requestedUndulatorPosition);
    average = (expectedUndulatorPosition + requestedUndulatorPosition) / 2;
    percentageDifference = (absDifference / average) * 100;
    if(percentageDifference > 1) {
        testResults_ << QString("%1 at %2eV (%3) Undulator position test failed - Expected:%4 Actual:%5").arg(name).arg(requiredEnergy).arg(gratingString).arg(expectedUndulatorPosition).arg(requestedUndulatorPosition);
    }

    absDifference = qAbs(expectedUndulatorStepValue - requestedUndulatorStepValue);
    average = (expectedUndulatorStepValue + requestedUndulatorStepValue) / 2;
    percentageDifference = (absDifference / average) * 100;
    if(percentageDifference > 1) {
        testResults_ << QString("%1 at %2eV (%3) Undulator step test failed - Expected:%4 Actual:%5").arg(name).arg(requiredEnergy).arg(gratingString).arg(expectedUndulatorStepValue).arg(requestedUndulatorStepValue);
    }

    absDifference = qAbs(expectedExitSlitPosition - requestedExitSlitPosition);
    average = (expectedExitSlitPosition + requestedExitSlitPosition) / 2;
    percentageDifference = (absDifference / average) * 100;
    if(percentageDifference > 1) {
        testResults_ << QString("%1 at %2eV (%3) Exit Slit test failed - Expected:%4 Actual:%5").arg(name).arg(requiredEnergy).arg(gratingString).arg(expectedExitSlitPosition).arg(requestedExitSlitPosition);
    }

}

void SGMMonochromatorInfoTest::testRequestEnergy()
{
    // Values derived from Dave's Spreadsheet (See GoogleDrive SGMUpgrade/Energy Coordinator/SGMEnergy.xlsx)
    // Undulator step values are derived from Iain's Spreadsheet (See GoogleDrive SGMUpgrade/Energy Coordinator/UndulatorValues.xls)

    // Carbon K Edge:
    testIndividualRequestEnergy("Carbon K Edge Start", 270.0, SGMGratingSupport::LowGrating, -412460.94, 358.199551, 13.2265442, -148306.40);
    testIndividualRequestEnergy("Carbon K Edge Middle", 295.0, SGMGratingSupport::LowGrating, -377507.29, 200.403381, 13.96618575, -139571.22);
    testIndividualRequestEnergy("Carbon K Edge End", 320.0, SGMGratingSupport::LowGrating, -348014.54, 100.4572035, 14.65750993, -131406.68);

    // Nitrogen K Edge:
    testIndividualRequestEnergy("Nitrogen K Edge Start", 380.0, SGMGratingSupport::LowGrating, -293064.87, 4.062753137, 16.16217837,-113636.53);
    testIndividualRequestEnergy("Nitrogen K Edge Middle", 410.0, SGMGratingSupport::LowGrating, -271621.10, 3.727639899, 16.85003997, -105512.88);
    testIndividualRequestEnergy("Nitrogen K Edge End", 440.0, SGMGratingSupport::LowGrating, -253101.48, 24.09068861, 17.50392332, -97790.51);

    // Oxygen K Edge:
    testIndividualRequestEnergy("Oxygen K Edge Start", 530.0, SGMGratingSupport::LowGrating, -210121.61, 166.3542454, 19.30862318, -76476.99);
    testIndividualRequestEnergy("Oxygen K Edge Middle", 545.0, SGMGratingSupport::LowGrating, -204338.45, 198.3497618, 19.59129459, -73138.64);
    testIndividualRequestEnergy("Oxygen K Edge End", 560.0, SGMGratingSupport::LowGrating, -198865.10, 232.1043969, 19.86977214, -69849.82);

    // Calcium L Edge:
    testIndividualRequestEnergy("Calcium L Edge Start", 345.0, SGMGratingSupport::MediumGrating, -590024.88, 1834.184986, 15.308185, -123722.20);
    testIndividualRequestEnergy("Calcium L Edge Middle", 355.0, SGMGratingSupport::MediumGrating, -573404.46, 1642.027233, 15.55847741, -120766.24);
    testIndividualRequestEnergy("Calcium L Edge End", 365.0, SGMGratingSupport::MediumGrating, -557694.75, 1473.127714, 15.80358308, -117871.54);

    // Titanium L Edge:
    testIndividualRequestEnergy("Titanium L Edge Start", 450.0, SGMGratingSupport::LowGrating, -247476.56, 34.56543835, 17.71524808, -95294.76);
    testIndividualRequestEnergy("Titanium L Edge Middle", 465.0, SGMGratingSupport::LowGrating, -239493.45, 53.21604579, 18.02659692, -91617.73);
    testIndividualRequestEnergy("Titanium L Edge End", 480.0, SGMGratingSupport::LowGrating, -232009.28, 75.02669983, 18.33163518, -88015.23);

    // Chromium L Edge:
    testIndividualRequestEnergy("Chromium L Edge Start", 565.0, SGMGratingSupport::LowGrating, -197105.23, 243.7180414, 19.96171999, -68763.91);
    testIndividualRequestEnergy("Chromium L Edge Middle", 585.0, SGMGratingSupport::LowGrating, -190366.59, 291.837129, 20.32541069, -64468.72);
    testIndividualRequestEnergy("Chromium L Edge End", 605.0, SGMGratingSupport::LowGrating, -184073.48, 342.3878768, 20.68302778, -60245.26);

    // Iron L Edge:
    testIndividualRequestEnergy("Iron L Edge Start", 690.0, SGMGratingSupport::MediumGrating, -295012.44, 70.28355603, 22.1484378, -42938.76);
    testIndividualRequestEnergy("Iron L Edge Middle", 720.0, SGMGratingSupport::MediumGrating, -282720.26, 70.35620468, 22.64942635, -27022.08);
    testIndividualRequestEnergy("Iron L Edge End", 750.0, SGMGratingSupport::MediumGrating, -271411.45, 77.59061352, 23.14434469, -31177.09);

    // Cobalt L Edge:
    // NOTE : Cobalt L Edge Start is the same as Iron L Edge End, so I'm omitting it
    testIndividualRequestEnergy("Cobalt L Edge Middle", 780.0, SGMGratingSupport::MediumGrating, -260972.54, 90.92797993, 23.63452204, -25388.09);
    testIndividualRequestEnergy("Cobalt L Edge End", 820.0, SGMGratingSupport::MediumGrating, -248242.18, 116.7468539, 24.28288187, -17730.95);

    //Nickel L Edge:
    testIndividualRequestEnergy("Nickel L Edge Start", 840.0, SGMGratingSupport::MediumGrating,  -242331.65, 132.6402691, 24.60563035, -13919.29);
    testIndividualRequestEnergy("Nickel L Edge Middle", 865.0, SGMGratingSupport::MediumGrating,  -235327.84, 154.965777, 25.00841512, -9162.40);
    testIndividualRequestEnergy("Nickel L Edge End", 890.0, SGMGratingSupport::MediumGrating,  -228717.51, 179.7613523, 25.41110095, -4406.68);

    // Magnesium K Edge:
    testIndividualRequestEnergy("Magnesium K Edge Start", 1290.0, SGMGratingSupport::HighGrating, -243735.55, 145.2066147, 32.4753518, 79022.19);
    testIndividualRequestEnergy("Magnesium K Edge Middle", 1315.0, SGMGratingSupport::HighGrating, -239101.80, 159.3760676, 33.0118301, 85358.00);
    testIndividualRequestEnergy("Magnesium K Edge End", 1340.0, SGMGratingSupport::HighGrating, -234640.94, 174.6471271, 33.57032489, 91953.83);

    // Aluminium K Edge
    testIndividualRequestEnergy("Aluminium K Edge Start", 1550.0, SGMGratingSupport::HighGrating, -202850.88, 337.8214953, 19.05357132, -79489.15);
    testIndividualRequestEnergy("Aluminium K Edge Middle", 1580.0, SGMGratingSupport::HighGrating,  -198999.28, 365.2307212, 19.24520489, -77225.96);
    testIndividualRequestEnergy("Aluminium K Edge End", 1610.0, SGMGratingSupport::HighGrating, -195291.22, 393.467824, 19.43479335, -74986.92);

    // Silicon K Edge
    testIndividualRequestEnergy("Silicon K Edge Start", 1830.0, SGMGratingSupport::HighGrating, -171813.59, 621.5737302, 20.77155529, -59199.75);
    testIndividualRequestEnergy("Silicon K Edge Middle", 1860.0, SGMGratingSupport::HighGrating, -169042.40, 655.0803091, 20.94762194, -57120.40);
    testIndividualRequestEnergy("Silicon K Edge End", 1890.0, SGMGratingSupport::HighGrating, -166359.19, 689.0727254, 21.12242885, -55055.93);
}

void SGMMonochromatorInfoTest::testResultantEnergy()
{
    SGMMonochromatorInfo testMonoInfo(240, SGMGratingSupport::LowGrating);

    if(!qFuzzyCompare(testMonoInfo.resultantEnergy(), 240)) {
        testResults_.append(QString("Resultant energy test failed - Expected: %1 Actual %2").arg(240).arg(testMonoInfo.resultantEnergy()));
    }
}
