#include "ALSBL8XESDetectorInfo.h"

ALSBL8XESDetectorInfo::ALSBL8XESDetectorInfo(QObject *parent) :
	AMDetectorInfo("xes", "XES Image Detector", parent)
{
	metaData_["centerEnergy"] = double(0.0);
	metaData_["grating"] = int(1);
	metaData_["angle"] = double(0.0);
	metaData_["posX"] = double(0.0);
	metaData_["posY"] = double(0.0);
	metaData_["posA"] = double(0.0);
	metaData_["slitWidth"] = double(0.0);
	metaData_["targetCounts"] = int(10000);
	metaData_["targetExposure"] = double(180);

	metaData_["pixelsX"] = int(1024);
	metaData_["pixelsY"] = int(32);
	metaData_["numGratings"] = int(4);
	metaData_["anodeWidthX"] = double(40);
	metaData_["gratingNames"] = QString("Grating 1,Grating 2,Grating 3,Grating 4").split(',');
	metaData_["slitPositionX"] = double(1138.161);
	metaData_["slitPositionY"] = double(180.420);
	AMDoubleList gParams;
	gParams << 600.0 << 1500.0 << 1000.0 << 1500.0;
	metaData_["gratingGrooveDensities"] = qVariantFromValue(gParams);

	gParams.clear();
	gParams << 4999.05 << 5000.2 << 10000.33 << 10000.0;
	metaData_["gratingRadii"] = qVariantFromValue(gParams);

	gParams.clear();
	gParams << 801.567 << 801.637 << 801.608 << 801.608;
	metaData_["gratingPositionsX"] = qVariantFromValue(gParams);

	gParams.clear();
	gParams << 102.767 << 102.718 << 102.505 << 102.336;
	metaData_["gratingPositionsY"] = qVariantFromValue(gParams);

	gParams.clear();
	gParams << -10.0815 << -10.2915 << -14.0385 << -14.0385;
	metaData_["zeroOrderRayAngles"] = qVariantFromValue(gParams);

	gParams.clear();
	gParams << 0 << 0 << 0 << 6.55034;
	metaData_["gratingCorrectionsDX"] = qVariantFromValue(gParams);

	gParams.clear();
	gParams << 0 << 0 << 0 << 3.62933;
	metaData_["gratingCorrectionsDY"] = qVariantFromValue(gParams);

	gParams.clear();
	gParams << 0 << 0 << 0 << -0.0100719;
	metaData_["gratingCorrectionsDA"] = qVariantFromValue(gParams);

	gParams.clear();
	gParams << 0 << 0 << 0 << 27.1196;
	metaData_["gratingCorrectionsGrooveDensity"] = qVariantFromValue(gParams);

	gParams.clear();
	gParams << 0 << 0 << 0 << -65.4955;
	metaData_["gratingCorrectionsRadii"] = qVariantFromValue(gParams);

	gParams.clear();
	gParams << 0 << 0 << 0 << -1.97679;
	metaData_["slitCorrectionsDX"] = qVariantFromValue(gParams);

	gParams.clear();
	gParams << 0 << 0 << 0 << 2.79737;
	metaData_["slitCorrectionsDY"] = qVariantFromValue(gParams);

}
