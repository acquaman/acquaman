#include "SGMMonochromatorInfo.h"

#include <cmath>
SGMMonochromatorInfo::SGMMonochromatorInfo(QObject *parent) :
    QObject(parent)
{
	setGratingTranslation(SGMGratingSupport::UnknownGrating);
	setGratingAngle(0.0);
	setUndulatorHarmonic(SGMUndulatorSupport::UnknownUndulatorHarmonic);
	setUndulatorPosition(0.0);
	setUndulatorOffset(0.0);
	setExitSlitPosition(0.0);

	connect(&errorValidator_, SIGNAL(validStateChanged(bool)), this, SIGNAL(errorStateChanged(bool)));
	connect(&errorValidator_, SIGNAL(failCountChanged(int)), this, SIGNAL(errorCountChanged()));

	connect(&warningValidator_, SIGNAL(validStateChanged(bool)), this, SIGNAL(warningStateChanged(bool)));
	connect(&warningValidator_, SIGNAL(failCountChanged(int)), this, SIGNAL(warningCountChanged()));
}

bool SGMMonochromatorInfo::hasErrors() const
{
	return !errorValidator_.isValid();
}

bool SGMMonochromatorInfo::hasWarnings() const
{
	return !warningValidator_.isValid();
}

QStringList SGMMonochromatorInfo::errorMessages() const
{
	return errorValidator_.failureMessages();
}

QStringList SGMMonochromatorInfo::warningMessages() const
{
	return warningValidator_.failureMessages();
}

double SGMMonochromatorInfo::resultantEnergy() const
{
	return energyFromGrating(gratingTranslation_, gratingAngle_);
}

SGMGratingSupport::GratingTranslation SGMMonochromatorInfo::gratingTranslation() const
{
	return gratingTranslation_;
}

void SGMMonochromatorInfo::setGratingTranslation(SGMGratingSupport::GratingTranslation gratingTranslation)
{
	if(gratingTranslation_ != gratingTranslation) {
		gratingTranslation_ = gratingTranslation;
		errorValidator_.updateValidity(MONOCHROMATOR_UNKNOWN_GRATING_TRANSLATION, gratingTranslation_ != SGMGratingSupport::UnknownGrating);
		emit gratingTranslationChanged(gratingTranslation_);
		emit energyChanged(resultantEnergy());
	}
}

double SGMMonochromatorInfo::gratingAngle() const
{
	return gratingAngle_;
}

void SGMMonochromatorInfo::setGratingAngle(double gratingAngle)
{
	if(gratingAngle_ != gratingAngle) {
		gratingAngle_ = gratingAngle;
		emit gratingAngleChanged(gratingAngle_);
		emit energyChanged(resultantEnergy());
	}
}

SGMUndulatorSupport::UndulatorHarmonic SGMMonochromatorInfo::undulatorHarmonic() const
{
	return undulatorHarmonic_;
}

void SGMMonochromatorInfo::setUndulatorHarmonic(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic)
{
	if(undulatorHarmonic_ != undulatorHarmonic) {
		undulatorHarmonic_ = undulatorHarmonic;
		errorValidator_.updateValidity(MONOCHROMATOR_UNKNOWN_UNDULATOR_HARMONIC, undulatorHarmonic_ != SGMUndulatorSupport::UnknownUndulatorHarmonic);
		emit undulatorHarmonicChanged(undulatorHarmonic_);
	}
}

double SGMMonochromatorInfo::undulatorPosition() const
{
	return undulatorPosition_;
}

void SGMMonochromatorInfo::setUndulatorPosition(double undulatorPosition)
{
	if(undulatorPosition_ != undulatorPosition) {
		undulatorPosition_ = undulatorPosition;
		emit undulatorPositionChanged(undulatorPosition);
	}
}

double SGMMonochromatorInfo::undulatorOffset() const
{
	return undulatorOffset_;
}

void SGMMonochromatorInfo::setUndulatorOffset(double undulatorOffset)
{
	if(undulatorOffset_ != undulatorOffset) {
		undulatorOffset_ = undulatorOffset;
		emit undulatorOffsetChanged(undulatorOffset_);
	}
}

double SGMMonochromatorInfo::exitSlitPosition() const
{
	return exitSlitPosition_;
}

void SGMMonochromatorInfo::setExitSlitPosition(double exitSlitPosition)
{
	if(exitSlitPosition_ != exitSlitPosition) {
		exitSlitPosition_ = exitSlitPosition;
		emit exitSlitPositionChanged(exitSlitPosition_);
	}
}

void SGMMonochromatorInfo::requestEnergy(double requestedEnergy, SGMGratingSupport::GratingTranslation gratingTranslation)
{
	setGratingTranslation(gratingTranslation);

	setGratingAngle(gratingAngleFromEnergy(gratingTranslation_, requestedEnergy));
	setUndulatorPosition(optimizedUndulatorPosition(requestedEnergy, undulatorHarmonic_, undulatorOffset_));
	setExitSlitPosition(optimizedExitSlitPosition(gratingTranslation_, requestedEnergy));
}

void SGMMonochromatorInfo::requestEnergy(double requestedEnergy, GratingTranslationOptimizationMode optimizationMode)
{
	requestEnergy(requestedEnergy, optimizedGrating(requestedEnergy, optimizationMode));
}

SGMGratingSupport::GratingTranslation SGMMonochromatorInfo::optimizedGrating(double requestedEnergy, SGMMonochromatorInfo::GratingTranslationOptimizationMode optimizationMode)
{
	// The grating which produces the largest flux at a given photon energy.
	if(optimizationMode == OptimizeFlux) {

		if(requestedEnergy < 700) {

			return SGMGratingSupport::LowGrating;
		} else if(requestedEnergy < 1300) {

			return SGMGratingSupport::MediumGrating;
		} else {

			return SGMGratingSupport::HighGrating;
		}
	} else if(optimizationMode == OptimizeResolution) {

		if (requestedEnergy > 650) {

			return SGMGratingSupport::HighGrating;
		} else if (requestedEnergy > 450) {

			return SGMGratingSupport::MediumGrating;
		} else {

			return SGMGratingSupport::LowGrating;
		}
	} else {

		return gratingTranslation_;
	}
}

#include <QDebug>
void SGMMonochromatorInfo::testValue(const QString &name, double requiredEnergy, SGMGratingSupport::GratingTranslation gratingTranslationSelection, double expectedAngleEncoderTarget, double expectedExitSlitPosition, double expectedUndulatorPosition, double expectedUndulatorStepValue)
{
	SGMMonochromatorInfo testMonoInfo;

	if(gratingTranslationSelection == SGMGratingSupport::HighGrating && requiredEnergy >= 1400) {
		testMonoInfo.setUndulatorHarmonic(SGMUndulatorSupport::ThirdHarmonic);
	} else {
		testMonoInfo.setUndulatorHarmonic(SGMUndulatorSupport::FirstHarmonic);
	}
	testMonoInfo.requestEnergy(requiredEnergy, gratingTranslationSelection);


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
		qDebug() << "\t" << name << " at " << requiredEnergy << "eV has errors - can't test";
		qDebug() << "Errors:";
		qDebug() << testMonoInfo.errorMessages();
		return;
	}

	if(testMonoInfo.hasWarnings()) {
		qDebug() << "\t" << name << " at " << requiredEnergy << "eV has warnings - tests continue";
		qDebug() << testMonoInfo.warningMessages();
	}

	// Do comparisons, outputting if the percentage difference from expected is > 1%
	absDifference = qAbs(expectedAngleEncoderTarget - angleEncoderTarget);
	average = (expectedAngleEncoderTarget + angleEncoderTarget) / 2;
	percentageDifference = (absDifference / average) * 100;
	if(percentageDifference >= 1) {
		qDebug() << "\t" << name << " at " << requiredEnergy << "eV (" << gratingString << ") Grating Difference = " << absDifference << " " << percentageDifference << "%";
		qDebug() << "\t\tExpected: " << expectedAngleEncoderTarget << "Actual: " << angleEncoderTarget;
	}

	absDifference = qAbs(expectedUndulatorPosition - requestedUndulatorPosition);
	average = (expectedUndulatorPosition + requestedUndulatorPosition) / 2;
	percentageDifference = (absDifference / average) * 100;
	if(percentageDifference > 1) {
		qDebug() << "\t" << name << " at " << requiredEnergy << "eV (" << gratingString << ") Undulator Position Difference = " << absDifference << " " << percentageDifference << "%";
		qDebug() << "\t\tExpected: " << expectedUndulatorPosition << "Actual: " << requestedUndulatorPosition;
	}

	absDifference = qAbs(expectedUndulatorStepValue - requestedUndulatorStepValue);
	average = (expectedUndulatorStepValue + requestedUndulatorStepValue) / 2;
	percentageDifference = (absDifference / average) * 100;
	if(percentageDifference > 1) {
		qDebug() << "\t" << name << " at " << requiredEnergy << "eV (" << gratingString << ") Undulator Step Difference = " << absDifference << " " << percentageDifference << "%";
		qDebug() << "\t\tExpected: " << expectedUndulatorStepValue << "Actual: " << requestedUndulatorStepValue;
	}

	absDifference = qAbs(expectedExitSlitPosition - requestedExitSlitPosition);
	average = (expectedExitSlitPosition + requestedExitSlitPosition) / 2;
	percentageDifference = (absDifference / average) * 100;
	if(percentageDifference > 1) {
		qDebug() << "\t" << name << " at " << requiredEnergy << "eV (" << gratingString << ") Exit Slit Difference = " << absDifference << " " << percentageDifference << "%";
		qDebug() << "\t\tExpected: " << expectedExitSlitPosition << "Actual: " << requestedExitSlitPosition;
	}
}

void SGMMonochromatorInfo::testValues()
{
	// Values derived from Dave's Spreadsheet (See GoogleDrive SGMUpgrade/Energy Coordinator/SGMEnergy.xlsx)
	// Undulator step values are derived from Iain's Spreadsheet (See GoogleDrive SGMUpgrade/Energy Coordinator/UndulatorValues.xls)

	// Carbon K Edge:
	testValue("Carbon K Edge Start", 270.0, SGMGratingSupport::LowGrating, -412460.94, 358.199551, 13.2265442, -148306.40);
	testValue("Carbon K Edge Middle", 295.0, SGMGratingSupport::LowGrating, -377507.29, 200.403381, 13.96618575, -139571.22);
	testValue("Carbon K Edge End", 320.0, SGMGratingSupport::LowGrating, -348014.54, 100.4572035, 14.65750993, -131406.68);

	// Nitrogen K Edge:
	testValue("Nitrogen K Edge Start", 380.0, SGMGratingSupport::LowGrating, -293064.87, 4.062753137, 16.16217837,-113636.53);
	testValue("Nitrogen K Edge Middle", 410.0, SGMGratingSupport::LowGrating, -271621.10, 3.727639899, 16.85003997, -105512.88);
	testValue("Nitrogen K Edge End", 440.0, SGMGratingSupport::LowGrating, -253101.48, 24.09068861, 17.50392332, -97790.51);

	// Oxygen K Edge:
	testValue("Oxygen K Edge Start", 530.0, SGMGratingSupport::LowGrating, -210121.61, 166.3542454, 19.30862318, -76476.99);
	testValue("Oxygen K Edge Middle", 545.0, SGMGratingSupport::LowGrating, -204338.45, 198.3497618, 19.59129459, -73138.64);
	testValue("Oxygen K Edge End", 560.0, SGMGratingSupport::LowGrating, -198865.10, 232.1043969, 19.86977214, -69849.82);

	// Calcium L Edge:
	testValue("Calcium L Edge Start", 345.0, SGMGratingSupport::MediumGrating, -590024.88, 1834.184986, 15.308185, -123722.20);
	testValue("Calcium L Edge Middle", 355.0, SGMGratingSupport::MediumGrating, -573404.46, 1642.027233, 15.55847741, -120766.24);
	testValue("Calcium L Edge End", 365.0, SGMGratingSupport::MediumGrating, -557694.75, 1473.127714, 15.80358308, -117871.54);

	// Titanium L Edge:
	testValue("Titanium L Edge Start", 450.0, SGMGratingSupport::LowGrating, -247476.56, 34.56543835, 17.71524808, -95294.76);
	testValue("Titanium L Edge Middle", 465.0, SGMGratingSupport::LowGrating, -239493.45, 53.21604579, 18.02659692, -91617.73);
	testValue("Titanium L Edge Start", 480.0, SGMGratingSupport::LowGrating, -232009.28, 75.02669983, 18.33163518, -88015.23);

	// Chromium L Edge:
	testValue("Chromium L Edge Start", 565.0, SGMGratingSupport::LowGrating, -197105.23, 243.7180414, 19.96171999, -68763.91);
	testValue("Chromium L Edge Middle", 585.0, SGMGratingSupport::LowGrating, -190366.59, 291.837129, 20.32541069, -64468.72);
	testValue("Chromium L Edge End", 605.0, SGMGratingSupport::LowGrating, -184073.48, 342.3878768, 20.68302778, -60245.26);

	// Iron L Edge:
	testValue("Iron L Edge Start", 690.0, SGMGratingSupport::MediumGrating, -295012.44, 70.28355603, 22.1484378, -42938.76);
	testValue("Iron L Edge Middle", 720.0, SGMGratingSupport::MediumGrating, -282720.26, 70.35620468, 22.64942635, -27022.08);
	testValue("Iron L Edge End", 750.0, SGMGratingSupport::MediumGrating, -271411.45, 77.59061352, 23.14434469, -31177.09);

	// Cobalt L Edge:
	// NOTE : Cobalt L Edge Start is the same as Iron L Edge End, so I'm omitting it
	testValue("Cobalt L Edge Middle", 780.0, SGMGratingSupport::MediumGrating, -260972.54, 90.92797993, 23.63452204, -25388.09);
	testValue("Cobalt L Edge End", 820.0, SGMGratingSupport::MediumGrating, -248242.18, 116.7468539, 24.28288187, -17730.95);

	//Nickel L Edge:
	testValue("Nickel L Edge Start", 840.0, SGMGratingSupport::MediumGrating,  -242331.65, 132.6402691, 24.60563035, -13919.29);
	testValue("Nickel L Edge Middle", 865.0, SGMGratingSupport::MediumGrating,  -235327.84, 154.965777, 25.00841512, -9162.40);
	testValue("Nickel L Edge End", 890.0, SGMGratingSupport::MediumGrating,  -228717.51, 179.7613523, 25.41110095, -4406.68);

	// Magnesium K Edge:
	testValue("Magnesium K Edge Start", 1290.0, SGMGratingSupport::HighGrating, -243735.55, 145.2066147, 32.4753518, 79022.19);
	testValue("Magnesium K Edge Middle", 1315.0, SGMGratingSupport::HighGrating, -239101.80, 159.3760676, 33.0118301, 85358.00);
	testValue("Magnesium K Edge End", 1340.0, SGMGratingSupport::HighGrating, -234640.94, 174.6471271, 33.57032489, 91953.83);

	// Aluminium K Edge
	testValue("Aluminium K Edge Start", 1550.0, SGMGratingSupport::HighGrating, -202850.88, 337.8214953, 19.05357132, -79489.15);
	testValue("Aluminium K Edge Middle", 1580.0, SGMGratingSupport::HighGrating,  -198999.28, 365.2307212, 19.24520489, -77225.96);
	testValue("Aluminium K Edge End", 1610.0, SGMGratingSupport::HighGrating, -195291.22, 393.467824, 19.43479335, -74986.92);

	// Silicon K Edge
	testValue("Silicon K Edge Start", 1830.0, SGMGratingSupport::HighGrating, -171813.59, 621.5737302, 20.77155529, -59199.75);
	testValue("Silicon K Edge Middle", 1860.0, SGMGratingSupport::HighGrating, -169042.40, 655.0803091, 20.94762194, -57120.40);
	testValue("Silicon K Edge End", 1890.0, SGMGratingSupport::HighGrating, -166359.19, 689.0727254, 21.12242885, -55055.93);
}

double SGMMonochromatorInfo::energyFromGrating(SGMGratingSupport::GratingTranslation gratingTranslationSelection, double gratingAngleEncoderTarget) const
{
	double spacing = SGMGratingSupport::spacing(gratingTranslationSelection);
	double c1 = SGMGratingSupport::c1(gratingTranslationSelection);
	double c2 = SGMGratingSupport::c2(gratingTranslationSelection);
	double radiusCurvatureOffset = SGMGratingSupport::radiusCurvatureOffset(gratingTranslationSelection);
	double thetaM = SGMGratingSupport::thetaM(gratingTranslationSelection);

	return 1e-9 * 1239.842 / ((2 * spacing * c1 * c2 * gratingAngleEncoderTarget) / radiusCurvatureOffset * cos(thetaM / 2));
}

double SGMMonochromatorInfo::gratingAngleFromEnergy(SGMGratingSupport::GratingTranslation gratingTranslationSelection, double energy) const
{
	double spacing = SGMGratingSupport::spacing(gratingTranslationSelection);
	double c1 = SGMGratingSupport::c1(gratingTranslationSelection);
	double c2 = SGMGratingSupport::c2(gratingTranslationSelection);
	double radiusCurvatureOffset = SGMGratingSupport::radiusCurvatureOffset(gratingTranslationSelection);
	double thetaM = SGMGratingSupport::thetaM(gratingTranslationSelection);

	return 1e-9 * 1239.842 / ((2 * spacing * c1 * c2 * energy) / radiusCurvatureOffset * cos(thetaM / 2));
}

double SGMMonochromatorInfo::optimizedUndulatorPosition(double energy, SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic, double undulatorOffset) const
{
	return (-1/0.14295709668) * log( (1/36.00511212946)*((1737.41045746644/(energy/int(undulatorHarmonic))) -1)) + undulatorOffset;
}

double SGMMonochromatorInfo::optimizedExitSlitPosition(SGMGratingSupport::GratingTranslation gratingTranslationSelection, double energy) const
{
	double spacing = SGMGratingSupport::spacing(gratingTranslationSelection);
	double thetaM = SGMGratingSupport::thetaM(gratingTranslationSelection);
	double xOffset = 4546; // Need to research this value. Seems to have been altered over time.
	double lambda = (1239.842 / energy) * 1.0e-9;
	double thetaI = asin((lambda / (2*spacing)) / cos(thetaM/2)) + (thetaM/2);
	double thetaD = -thetaM + thetaI;

	return -xOffset + (pow(cos(thetaD),2)) / (((cos(thetaD + thetaM) + cos(thetaD)) / 70480) - (pow(cos(thetaD + thetaM),2))/1500);
}



